// ReSharper disable CppClangTidyModernizeAvoidBind
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <config_protocol/config_protocol_server.h>
#include <config_protocol/config_protocol_client.h>
#include "test_utils.h"
#include <config_protocol/config_client_object_ptr.h>
#include "coreobjects/argument_info_factory.h"
#include "coreobjects/callable_info_factory.h"
#include "opendaq/context_factory.h"

using namespace daq;
using namespace config_protocol;
using namespace testing;
using namespace std::placeholders;


class ConfigProtocolIntegrationTest : public Test
{
public:
    void SetUp() override
    {
        serverDevice = test_utils::createServerDevice();
        serverDevice.asPtrOrNull<IPropertyObjectInternal>().enableCoreEventTrigger();
        server = std::make_unique<ConfigProtocolServer>(serverDevice, std::bind(&ConfigProtocolIntegrationTest::serverNotificationReady, this, std::placeholders::_1));

        clientContext = NullContext();
        client = std::make_unique<ConfigProtocolClient>(clientContext, std::bind(&ConfigProtocolIntegrationTest::sendRequest, this, std::placeholders::_1), nullptr);

        client->connect();
        client->getDevice().asPtr<IPropertyObjectInternal>().enableCoreEventTrigger();
        clientDevice = client->getDevice();
    }

    static StringPtr serializeComponent(const ComponentPtr& component)
    {
        const auto serializer = JsonSerializer(True);
        component.serialize(serializer);
        const auto str = serializer.getOutput();
        return str;
    }
    
    PacketBuffer sendRequest(const PacketBuffer& requestPacket) const
    {
        auto replyPacket = server->processRequestAndGetReply(requestPacket);
        return replyPacket;
    }
    
    void serverNotificationReady(const PacketBuffer& notificationPacket) const
    {
        client->triggerNotificationPacket(notificationPacket);
    }

protected:
    DevicePtr serverDevice;
    DevicePtr clientDevice;
    std::unique_ptr<ConfigProtocolServer> server;
    std::unique_ptr<ConfigProtocolClient> client;
    ContextPtr clientContext;
    BaseObjectPtr notificationObj;

};

TEST_F(ConfigProtocolIntegrationTest, Connect)
{
    const auto serverDeviceSerialized = serializeComponent(serverDevice);
    const auto clientDeviceSerialized = serializeComponent(clientDevice);
    ASSERT_EQ(serverDeviceSerialized, clientDeviceSerialized);
}

TEST_F(ConfigProtocolIntegrationTest, RemoteGlobalIds)
{
    const auto serverDeviceSerialized = serializeComponent(serverDevice);
    ASSERT_EQ(clientDevice.asPtr<IConfigClientObject>(true).getRemoteGlobalId(), serverDevice.getGlobalId());
    ASSERT_EQ(clientDevice.getDevices()[0].asPtr<IConfigClientObject>(true).getRemoteGlobalId(), serverDevice.getDevices()[0].getGlobalId());
    ASSERT_EQ(clientDevice.getDevices()[0].getFunctionBlocks()[0].asPtr<IConfigClientObject>(true).getRemoteGlobalId(),
              serverDevice.getDevices()[0].getFunctionBlocks()[0].getGlobalId());
    ASSERT_EQ(clientDevice.getDevices()[0].getFunctionBlocks()[0].getSignals()[0].asPtr<IConfigClientObject>(true).getRemoteGlobalId(),
              serverDevice.getDevices()[0].getFunctionBlocks()[0].getSignals()[0].getGlobalId());
    ASSERT_EQ(clientDevice.getChannels()[0].getSignals()[0].asPtr<IConfigClientObject>(true).getRemoteGlobalId(),
              serverDevice.getChannels()[0].getSignals()[0].getGlobalId());
}

TEST_F(ConfigProtocolIntegrationTest, ConnectWithParent)
{
    const auto serverDeviceSerialized = serializeComponent(serverDevice);
    
    ConfigProtocolClient clientNew(clientContext, std::bind(&ConfigProtocolIntegrationTest::sendRequest, this, std::placeholders::_1), nullptr);

    const auto parentComponent = Component(clientContext, nullptr, "cmp");

    clientNew.connect(parentComponent);
    const auto clientDevice = clientNew.getDevice();

    ASSERT_EQ(clientDevice.asPtr<IConfigClientObject>(true).getRemoteGlobalId(), serverDevice.getGlobalId());
    ASSERT_EQ(clientDevice.getGlobalId(), "/cmp/" + clientDevice.getLocalId().toStdString());
}

void checkComponentForConfigClientObject(const ComponentPtr& component)
{
    if (!component.supportsInterface<IConfigClientObject>())
        throw std::runtime_error(fmt::format("Component {} not a config client object", component.getGlobalId()));

    const auto folder = component.asPtrOrNull<IFolder>(true);
    if (folder.assigned())
        for (const auto& item: folder.getItems())
            checkComponentForConfigClientObject(item);
}

TEST_F(ConfigProtocolIntegrationTest, CheckConfigClientObject)
{
    const auto serverDeviceSerialized = serializeComponent(serverDevice);
    checkComponentForConfigClientObject(clientDevice);
}

TEST_F(ConfigProtocolIntegrationTest, GetInitialPropertyValue)
{
    serverDevice.getChannels()[0].setPropertyValue("StrProp", "SomeValue");

    const auto serverDeviceSerialized = serializeComponent(serverDevice);
    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrProp"), clientDevice.getChannels()[0].getPropertyValue("StrProp"));

    const auto clientDeviceSerialized = serializeComponent(clientDevice);
    ASSERT_EQ(serverDeviceSerialized, clientDeviceSerialized);
}


TEST_F(ConfigProtocolIntegrationTest, SetPropertyValue)
{
    clientDevice.getChannels()[0].setPropertyValue("StrProp", "SomeValue");

    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrProp"), "SomeValue");
    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrProp"), clientDevice.getChannels()[0].getPropertyValue("StrProp"));
}

TEST_F(ConfigProtocolIntegrationTest, SetProtectedPropertyValue)
{
    ASSERT_THROW(clientDevice.getChannels()[0].setPropertyValue("StrPropProtected", "SomeValue"), AccessDeniedException);

    clientDevice.getChannels()[0].asPtr<IPropertyObjectProtected>(true).setProtectedPropertyValue("StrPropProtected", "SomeValue");

    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrPropProtected"), "SomeValue");
    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrPropProtected"), clientDevice.getChannels()[0].getPropertyValue("StrPropProtected"));
}

TEST_F(ConfigProtocolIntegrationTest, ClearPropertyValue)
{
    serverDevice.getChannels()[0].setPropertyValue("StrProp", "SomeValue");

    clientDevice.getChannels()[0].clearPropertyValue("StrProp");
    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrProp"), "-");
    ASSERT_EQ(serverDevice.getChannels()[0].getPropertyValue("StrProp"), clientDevice.getChannels()[0].getPropertyValue("StrProp"));
}

TEST_F(ConfigProtocolIntegrationTest, CallFuncProp)
{
    const auto serverCh = serverDevice.getChannels()[0];
    const auto funcProp =
        FunctionPropertyBuilder("FuncProp", FunctionInfo(ctInt, List<IArgumentInfo>(ArgumentInfo("A", ctInt), ArgumentInfo("B", ctInt))))
            .setReadOnly(True)
            .build();
    serverCh.addProperty(funcProp);

    serverCh.asPtr<IPropertyObjectProtected>(true).setProtectedPropertyValue("FuncProp", Function([](Int a, Int b) { return a + b; }));

    const auto clientFuncPropValue = clientDevice.getChannels()[0].getPropertyValue("FuncProp");

    Int r = clientFuncPropValue.call(2, 4);
    ASSERT_EQ(r, 6);
}

TEST_F(ConfigProtocolIntegrationTest, CallProcProp)
{
    const auto serverCh = serverDevice.getChannels()[0];
    const auto procProp =
        FunctionPropertyBuilder("ProcProp", ProcedureInfo())
            .setReadOnly(True)
            .build();
    serverCh.addProperty(procProp);

    bool called = false;
    serverCh.asPtr<IPropertyObjectProtected>(true).setProtectedPropertyValue("ProcProp", Procedure([&called] { called = true; }));

    const auto clientProcPropValue = clientDevice.getChannels()[0].getPropertyValue("ProcProp");

    clientProcPropValue.dispatch();
    ASSERT_TRUE(called);
}

TEST_F(ConfigProtocolIntegrationTest, GetAvailableFunctionBlockTypes)
{
    const auto serverSubDevice = serverDevice.getDevices()[0];
    const auto clientSubDevice = clientDevice.getDevices()[0];

    const auto fbTypes = clientSubDevice.getAvailableFunctionBlockTypes();
    ASSERT_EQ(fbTypes, serverSubDevice.getAvailableFunctionBlockTypes());
}

TEST_F(ConfigProtocolIntegrationTest, AddFunctionBlockNotFound)
{
    const auto serverSubDevice = serverDevice.getDevices()[0];
    const auto clientSubDevice = clientDevice.getDevices()[0];

    ASSERT_THROW(clientSubDevice.addFunctionBlock("someFb"), NotFoundException);
}
TEST_F(ConfigProtocolIntegrationTest, AddFunctionBlock)
{
    serverDevice.asPtr<IPropertyObjectInternal>().disableCoreEventTrigger();
    const auto serverSubDevice = serverDevice.getDevices()[0];
    const auto clientSubDevice = clientDevice.getDevices()[0];
    const auto config = PropertyObject();
    config.addProperty(StringPropertyBuilder("Param", "Value").build());

    const auto fb = clientSubDevice.addFunctionBlock("mockfb1", config);

    const auto globalId1 = fb.getGlobalId();
    const auto globalId2 = fb.asPtr<IConfigClientObject>().getRemoteGlobalId();
    const auto globalId3 = serverSubDevice.getFunctionBlocks()[1].getGlobalId();

    ASSERT_EQ(fb, clientSubDevice.getFunctionBlocks()[1]);

    ASSERT_EQ(fb.asPtr<IConfigClientObject>().getRemoteGlobalId(), serverSubDevice.getFunctionBlocks()[1].getGlobalId());
}

TEST_F(ConfigProtocolIntegrationTest, AddFunctionBlockWithEvent)
{
    const auto serverSubDevice = serverDevice.getDevices()[0];
    const auto clientSubDevice = clientDevice.getDevices()[0];
    const auto config = PropertyObject();
    config.addProperty(StringPropertyBuilder("Param", "Value").build());

    const auto fb = clientSubDevice.addFunctionBlock("mockfb1", config);

    const auto globalId1 = fb.getGlobalId();
    const auto globalId2 = fb.asPtr<IConfigClientObject>().getRemoteGlobalId();
    const auto globalId3 = serverSubDevice.getFunctionBlocks()[1].getGlobalId();

    ASSERT_EQ(fb, clientSubDevice.getFunctionBlocks()[1]);

    ASSERT_EQ(fb.asPtr<IConfigClientObject>().getRemoteGlobalId(), serverSubDevice.getFunctionBlocks()[1].getGlobalId());
}

TEST_F(ConfigProtocolIntegrationTest, GetInitialStructPropertyValue)
{
    ASSERT_EQ(serverDevice.getPropertyValue("StructProp"), clientDevice.getPropertyValue("StructProp"));
}

TEST_F(ConfigProtocolIntegrationTest, SetStructPropertyValue)
{
    const auto structMembers = Dict<IString, IBaseObject>({{"string", "bar1"}, {"integer", 11}, {"float", 5.223}});
    const auto structVal = Struct("FooStruct", structMembers, serverDevice.getContext().getTypeManager());
    serverDevice.setPropertyValue("StructProp", structVal);

    ASSERT_EQ(serverDevice.getPropertyValue("StructProp"), structVal);
    ASSERT_EQ(serverDevice.getPropertyValue("StructProp"), clientDevice.getPropertyValue("StructProp"));
}
#include <testutils/testutils.h>
#include <native_streaming_client_module/module_dll.h>
#include <native_streaming_client_module/version.h>
#include <gmock/gmock.h>

#include <opendaq/module_ptr.h>
#include <coretypes/common.h>

#include <opendaq/context_factory.h>
#include <coreobjects/property_factory.h>
#include <coreobjects/property_object_factory.h>
#include <opendaq/device_info_factory.h>

using NativeStreamingClientModuleTest = testing::Test;
using namespace daq;

static ModulePtr CreateModule()
{
    ModulePtr module;
    createModule(&module, NullContext());
    return module;
}

TEST_F(NativeStreamingClientModuleTest, CreateModule)
{
    IModule* module = nullptr;
    ErrCode errCode = createModule(&module, NullContext());
    ASSERT_TRUE(OPENDAQ_SUCCEEDED(errCode));

    ASSERT_NE(module, nullptr);
    module->releaseRef();
}

TEST_F(NativeStreamingClientModuleTest, ModuleName)
{
    auto module = CreateModule();
    ASSERT_EQ(module.getName(), "openDAQ native streaming client module");
}

TEST_F(NativeStreamingClientModuleTest, VersionAvailable)
{
    auto module = CreateModule();
    ASSERT_TRUE(module.getVersionInfo().assigned());
}

TEST_F(NativeStreamingClientModuleTest, VersionCorrect)
{
    auto module = CreateModule();
    auto version = module.getVersionInfo();

    ASSERT_EQ(version.getMajor(), NATIVE_STREAM_CL_MODULE_MAJOR_VERSION);
    ASSERT_EQ(version.getMinor(), NATIVE_STREAM_CL_MODULE_MINOR_VERSION);
    ASSERT_EQ(version.getPatch(), NATIVE_STREAM_CL_MODULE_PATCH_VERSION);
}

TEST_F(NativeStreamingClientModuleTest, EnumerateDevices)
{
    auto module = CreateModule();

    ListPtr<IDeviceInfo> deviceInfo;
    ASSERT_NO_THROW(deviceInfo = module.getAvailableDevices());
}

TEST_F(NativeStreamingClientModuleTest, CreateDeviceConnectionStringNull)
{
    auto module = CreateModule();

    DevicePtr device;
    ASSERT_THROW(device = module.createDevice(nullptr, nullptr), ArgumentNullException);
}

TEST_F(NativeStreamingClientModuleTest, CreateDeviceConnectionFailed)
{
    auto module = CreateModule();

    ASSERT_THROW(module.createDevice("daq.ns://127.0.0.1", nullptr), NotFoundException);
    ASSERT_THROW(module.createDevice("daq.nd://127.0.0.1", nullptr), NotFoundException);
}

TEST_F(NativeStreamingClientModuleTest, CreateStreamingWithNullArguments)
{
    auto module = CreateModule();

    DevicePtr device;
    ASSERT_THROW(device = module.createStreaming(nullptr, nullptr), ArgumentNullException);
}
//
//TEST_F(NativeStreamingClientModuleTest, CreateConnectionStringIgnored)
//{
//    auto context = NullContext();
//    ModulePtr module;
//    createModule(&module, context);
//
//    StringPtr connectionString;
//    ServerCapabilityConfigPtr serverCapability = ServerCapability("test", "test", ProtocolType::Unknown);
//    ASSERT_NO_THROW(connectionString = module.createConnectionString(serverCapability));
//    ASSERT_FALSE(connectionString.assigned());
//}
//
//TEST_F(NativeStreamingClientModuleTest, CreateStreamingConnectionString)
//{
//    auto context = NullContext();
//    ModulePtr module;
//    createModule(&module, context);
//
//    StringPtr connectionString;
//    ServerCapabilityConfigPtr serverCapability = ServerCapability("opendaq_native_streaming", "openDAQ Native Streaming", ProtocolType::Streaming);
//    ASSERT_THROW(module.createConnectionString(serverCapability), InvalidParameterException);
//
//    serverCapability.addAddress("123.123.123.123");
//    ASSERT_EQ(module.createConnectionString(serverCapability), "daq.ns://123.123.123.123:7420");
//
//    serverCapability.setPort(1234);
//    ASSERT_NO_THROW(connectionString = module.createConnectionString(serverCapability));
//    ASSERT_EQ(connectionString, "daq.ns://123.123.123.123:1234");
//
//    serverCapability.addProperty(StringProperty("Path", "/path"));
//    ASSERT_NO_THROW(connectionString = module.createConnectionString(serverCapability));
//    ASSERT_EQ(connectionString, "daq.ns://123.123.123.123:1234/path");
//}
//
//TEST_F(NativeStreamingClientModuleTest, CreateDeviceConnectionString)
//{
//    auto context = NullContext();
//    ModulePtr module;
//    createModule(&module, context);
//
//    StringPtr connectionString;
//    ServerCapabilityConfigPtr serverCapability = ServerCapability("opendaq_native_config", "openDAQ Native Configuration", ProtocolType::ConfigurationAndStreaming);
//    ASSERT_THROW(module.createConnectionString(serverCapability), InvalidParameterException);
//
//    serverCapability.addAddress("123.123.123.123");
//    ASSERT_EQ(module.createConnectionString(serverCapability), "daq.nd://123.123.123.123:7420");
//
//    serverCapability.setPort(1234);
//    ASSERT_NO_THROW(connectionString = module.createConnectionString(serverCapability));
//    ASSERT_EQ(connectionString, "daq.nd://123.123.123.123:1234");
//
//    serverCapability.addProperty(StringProperty("Path", "/path"));
//    ASSERT_NO_THROW(connectionString = module.createConnectionString(serverCapability));
//    ASSERT_EQ(connectionString, "daq.nd://123.123.123.123:1234/path");
//}

TEST_F(NativeStreamingClientModuleTest, GetAvailableComponentTypes)
{
    const auto module = CreateModule();

    DictPtr<IString, IFunctionBlockType> functionBlockTypes;
    ASSERT_NO_THROW(functionBlockTypes = module.getAvailableFunctionBlockTypes());
    ASSERT_EQ(functionBlockTypes.getCount(), 0u);

    DictPtr<IString, IDeviceType> deviceTypes;
    ASSERT_NO_THROW(deviceTypes = module.getAvailableDeviceTypes());
    ASSERT_EQ(deviceTypes.getCount(), 2u);
    ASSERT_TRUE(deviceTypes.hasKey("opendaq_native_streaming"));
    ASSERT_EQ(deviceTypes.get("opendaq_native_streaming").getId(), "opendaq_native_streaming");
    ASSERT_TRUE(deviceTypes.hasKey("opendaq_native_config"));
    ASSERT_EQ(deviceTypes.get("opendaq_native_config").getId(), "opendaq_native_config");

    DictPtr<IString, IServerType> serverTypes;
    ASSERT_NO_THROW(serverTypes = module.getAvailableServerTypes());
    ASSERT_EQ(serverTypes.getCount(), 0u);
}

TEST_F(NativeStreamingClientModuleTest, DefaultDeviceConfig)
{
    const auto module = CreateModule();

    DictPtr<IString, IDeviceType> deviceTypes;
    ASSERT_NO_THROW(deviceTypes = module.getAvailableDeviceTypes());
    ASSERT_EQ(deviceTypes.getCount(), 2u);

    ASSERT_TRUE(deviceTypes.hasKey("opendaq_native_config"));
    auto deviceConfig = deviceTypes.get("opendaq_native_config").createDefaultConfig();
    ASSERT_TRUE(deviceConfig.assigned());

    ASSERT_TRUE(deviceTypes.hasKey("opendaq_native_streaming"));
    auto pseudoDeviceConfig = deviceTypes.get("opendaq_native_streaming").createDefaultConfig();
    ASSERT_TRUE(pseudoDeviceConfig.assigned());
}

class ConnectionStringTest : public NativeStreamingClientModuleTest,
                             public testing::WithParamInterface<StringPtr>
{
};

TEST_P(ConnectionStringTest, CreateDeviceWrongConnectionString)
{
    auto module = CreateModule();

    StringPtr connectionString = GetParam();

    ASSERT_THROW(module.createDevice(connectionString, nullptr), InvalidParameterException);
}

TEST_P(ConnectionStringTest, CreateStreamingWrongConnectionString)
{
    auto module = CreateModule();

    StringPtr connectionString = GetParam();

    ASSERT_THROW(module.createStreaming(connectionString, nullptr), InvalidParameterException);
}

INSTANTIATE_TEST_SUITE_P(
    ConnectionString,
    ConnectionStringTest,
    testing::Values(
        "",
        "drfrfgt",
        "daq.opcua://device8",
        "daqref://devicett3axxr1",
        "daq.opcua://devicett3axxr1"
        "daq.ns://",
        "daq.ns:///",
        "daq.opcua://[::1]"
    )
);

//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//
//     RTGen (PythonGenerator).
// </auto-generated>
//------------------------------------------------------------------------------

/*
 * Copyright 2022-2023 Blueberry d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "py_opendaq/py_opendaq.h"
#include "py_core_types/py_converter.h"

PyDaqIntf<daq::ISignal, daq::IComponent> declareISignal(pybind11::module_ m)
{
    return wrapInterface<daq::ISignal, daq::IComponent>(m, "ISignal");
}

void defineISignal(pybind11::module_ m, PyDaqIntf<daq::ISignal, daq::IComponent> cls)
{
    cls.doc() = "A signal with an unique ID that sends event/data packets through connections to input ports the signal is connected to.";

    cls.def_property("public",
        [](daq::ISignal *object)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            return objectPtr.getPublic();
        },
        [](daq::ISignal *object, const bool isPublic)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            objectPtr.setPublic(isPublic);
        },
        "Returns true if the signal is public; false otherwise. / Sets the signal to be either public or private.");
    cls.def_property_readonly("descriptor",
        [](daq::ISignal *object)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            return objectPtr.getDescriptor().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the signal's data descriptor.");
    cls.def_property_readonly("domain_signal",
        [](daq::ISignal *object)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            return objectPtr.getDomainSignal().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the signal that carries its domain data.");
    cls.def_property_readonly("related_signals",
        [](daq::ISignal *object)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            return objectPtr.getRelatedSignals().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets a list of related signals.");
    cls.def_property_readonly("connections",
        [](daq::ISignal *object)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            return objectPtr.getConnections().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the list of connections to input ports formed by the signal.");
    cls.def_property("name",
        nullptr,
        [](daq::ISignal *object, const std::string& name)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            objectPtr.setName(name);
        },
        "Sets the name of the signal.");
    cls.def_property("description",
        [](daq::ISignal *object)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            return objectPtr.getDescription().toStdString();
        },
        [](daq::ISignal *object, const std::string& description)
        {
            const auto objectPtr = daq::SignalPtr::Borrow(object);
            objectPtr.setDescription(description);
        },
        "Gets the description of the signal. / Sets the description of the signal.");
}
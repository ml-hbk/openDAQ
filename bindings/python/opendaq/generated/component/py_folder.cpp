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

PyDaqIntf<daq::IFolder, daq::IComponent> declareIFolder(pybind11::module_ m)
{
    return wrapInterface<daq::IFolder, daq::IComponent>(m, "IFolder");
}

void defineIFolder(pybind11::module_ m, PyDaqIntf<daq::IFolder, daq::IComponent> cls)
{
    cls.doc() = "Acts as a container for other components";

    cls.def_property_readonly("items",
        [](daq::IFolder *object)
        {
            const auto objectPtr = daq::FolderPtr::Borrow(object);
            return objectPtr.getItems().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the list of the items in the folder.");
    cls.def_property_readonly("empty",
        [](daq::IFolder *object)
        {
            const auto objectPtr = daq::FolderPtr::Borrow(object);
            return objectPtr.isEmpty();
        },
        "Returns True if the folder is empty.");
    cls.def("has_item",
        [](daq::IFolder *object, const std::string& localId)
        {
            const auto objectPtr = daq::FolderPtr::Borrow(object);
            return objectPtr.hasItem(localId);
        },
        py::arg("local_id"),
        "Returns True if the folder has an item with local ID.");
    cls.def("get_item",
        [](daq::IFolder *object, const std::string& localId)
        {
            const auto objectPtr = daq::FolderPtr::Borrow(object);
            return objectPtr.getItem(localId).detach();
        },
        py::arg("local_id"),
        "Gets the item component with the specified localId.");
}
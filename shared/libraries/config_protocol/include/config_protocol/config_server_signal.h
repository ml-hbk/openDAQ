/*
 * Copyright 2022-2024 Blueberry d.o.o.
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

#pragma once
#include <opendaq/device_ptr.h>
#include <opendaq/signal_ptr.h>

namespace daq::config_protocol
{

class ConfigServerSignal
{
public:
    static BaseObjectPtr getLastValue(const SignalPtr& signal, const ParamsDictPtr& params);
};

inline BaseObjectPtr ConfigServerSignal::getLastValue(const SignalPtr& signal, const ParamsDictPtr& /*params*/)
{
    const auto value = signal.getLastValue();
    return value;
}
}
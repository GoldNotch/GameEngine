// Copyright 2023 JohnCorn
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <cstdio>
#include <thread>
#include <cmath>
//#define APP_IMPLEMENTATION
#include <Engine.h>


struct USAPP_API USApp
{

};

USAPP_API USAppPtr usApp_Create()
{
	return new USApp();
}

USAPP_API void usApp_Tick(USAppPtr app)
{
	std::printf("Empty tick\n");
}

USAPP_API void usApp_Destroy(USAppPtr app)
{
	delete app;
}
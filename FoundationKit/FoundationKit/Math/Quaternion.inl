/**
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the FoundationKit project
 */
#include "Quaternion.h"

NS_FK_BEGIN

inline const Quaternion Quaternion::operator*(const Quaternion& q) const
{
    Quaternion result(*this);
    result.multiply(q);
    return result;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    multiply(q);
    return *this;
}

NS_FK_END

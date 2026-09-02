#pragma once

class BaseClass;

namespace Quad
{

// 변경된 프로퍼티를 런타임/에디터 바로 적용
// 더티 플래그 설정
void CommitInspectorEdit(BaseClass *target);

} // namespace Quad
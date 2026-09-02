add_library(Physics SHARED
PhysicsDllMacro.h
 "PhysicsWorld.h" "PhysicsWorld.cpp" "PhysicsScene.h" "PhysicsScene.cpp" "PhysicsType.h" "Collider.h" "Collider.cpp" "Contact.cpp" "Contact.h")



target_compile_definitions(Physics PRIVATE PHYSICS_EXPORT)


target_link_libraries(Physics PUBLIC compiler_flags headerDir CoreAllocator CoreBase CoreMath Utility)


target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
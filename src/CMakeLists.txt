file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SOURCE *.cpp)
file(GLOB HEADER *.hpp)

add_executable(grabidentifiers ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${HEADER} ${SOURCE})
target_link_libraries(grabidentifiers srcsaxeventdispatch srcsax_static ${LIBXML2_LIBRARIES})
file(GLOB DISPATCHER_SOURCE dispatcher/*.cpp)
file(GLOB DISPATCHER_HEADER dispatcher/*.hpp)

file(GLOB SLICE_SOURCE *.cpp)
file(GLOB SLICE_HEADER *.hpp)

add_executable(grabidentifiers ${DISPATCHER_SOURCE} ${DISPATCHER_HEADER} ${SLICE_HEADER} ${SLICE_SOURCE})
target_link_libraries(grabidentifiers srcsaxeventdispatch srcsax_static ${LIBXML2_LIBRARIES})
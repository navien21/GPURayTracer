grt_env = Environment()
grt_env.Decider("MD5-timestamp")

grt_env.Append(CPPPATH = ['inc'])

grt_src = """
src/Scene.cpp
src/Image.cpp
src/GPUManager.cpp
""".split()

grt_lib = grt_env.StaticLibrary('GRT', grt_src)

grt_env.Append(LIBS = [grt_lib])

grt_env.Program('grt', 'src/main.cpp')

# Build Test Progam
test_grt_srcs = """
test/test_suite.cpp
test/SceneTest.cpp
""".split()

test_grt_env = grt_env.Clone()
test_grt_env.Append(CPPPATH = ['/pkg/gtest/include'])
#test_env.Append(LIBPATH = ['.','/usr/local/lib','/opt/local/lib'])
test_grt_env.Append(LIBS = ['gtest'])

test_grt_env.Program('TestGRT', test_grt_srcs);

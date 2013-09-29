grt_env = Environment()
grt_env.Decider("MD5-timestamp")

grt_env.Append(CPPPATH = ['inc'])

grt_src = """
src/Scene.cpp
src/Image.cpp
src/GPUManager.cpp
src/main.cpp
""".split()

grt_env.Program('grt', grt_src)

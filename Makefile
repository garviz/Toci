all:
	mpic++ -Wno-write-strings toci.cpp access.cpp construct.cpp mapfile.cpp space.cpp  seterror.cpp maxmatopt.cpp procopt.cpp safescpy.cpp maxmatinp.cpp procmaxmat.cpp multiseq.cpp findmaxmat.cpp findmumcand.cpp cleanMUMcand.cpp streedbg.cpp scanpref.cpp linkloc.cpp dfs.cpp depthtab.cpp distribute.cpp -o toci

clean:
	rm toci

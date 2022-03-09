CXX = clang++
CXXFLAGS = -std=c++11 -Wall
FW = -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2
FILE = main

run: compile # call with FILE=<filename>
	@./$(FILE)

compile: $(FILE).cpp
	@$(CXX) $(FILE).cpp -o $(FILE) $(FW) $(CXXFLAGS)

all: gol.cpp slide.cpp snek.cpp sorts.cpp waterfall.cpp zelda.cpp
	@$(CXX) gol.cpp -o gol $(FW) $(CXXFLAGS)
	@$(CXX) slide.cpp -o slide $(FW) $(CXXFLAGS)
	@$(CXX) snek.cpp -o snek $(FW) $(CXXFLAGS)
	@$(CXX) sorts.cpp -o sorts $(FW) $(CXXFLAGS)
	@$(CXX) waterfall.cpp -o waterfall $(FW) $(CXXFLAGS)
	@$(CXX) lelda.cpp -o lelda $(FW) $(CXXFLAGS)

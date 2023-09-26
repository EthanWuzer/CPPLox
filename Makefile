CXX      := g++
CXXFLAGS := -ggdb -std=c++17
CPPFLAGS := -MMD

COMPILE  := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

mylox:
	@echo "Compiling..."
	@$(COMPILE) source/cpplox.cpp -o mylox
	@echo "Compiled."

challenge081:
	@$(COMPILE) source/cpplox.cpp -o mylox

challenge08O:
	@$(COMPILE) source/cpplox.cpp -o mylox

challenge093:
	@$(COMPILE) source/cpplox.cpp -o mylox

challenge09O:
	@$(COMPILE) source/cpplox.cpp -o mylox

.PHONY: clean
clean:
	@echo "Cleaning..."
	@rm -f *.d *.o mylox
	@echo "Cleaned."
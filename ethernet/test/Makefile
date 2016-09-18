export SHELL := /bin/bash -e
GTEST_DIR = ../../googletest/googletest
CXX := g++
CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -std=c++14 -g -Wall -Wextra -pthread


TESTS := test_rgb_url

test: $(TESTS)
	$(foreach t,$^,./$(t);)


clean:
	$(RM) -f $(foreach t,$(TESTS),$(t) $(t).o) gtest_main.a


test_%: test_%.cpp gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@


gtest_main.a:
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc
	$(AR) $(ARFLAGS) gtest_main.a gtest-all.o gtest_main.o
	$(RM) gtest-all.o gtest_main.o

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -I./includes -I/opt/homebrew/opt/openssl/include
LDFLAGS = -L/opt/homebrew/opt/openssl/lib -lssl -lcrypto

# Source files
SRCS = main.cpp modules/resolveWeb.cpp
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = dnsResolv

# Default rule
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

CXX = g++

all: error_detection

error_detection: error_detection.cpp
	$(CXX) error_detection.cpp -o error_detection

clean:
	rm error_detection
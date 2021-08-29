if [ ! -f "rain" ]; then
	g++ rain.cpp -o rain
fi

./rain `tput cols` `tput lines`

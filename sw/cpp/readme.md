

### dependencies

    sudo apt-get install -y libi2c-dev

### compilation 


    g++ -std=c++17 -O2 -Wall i2c_read.cpp -o i2c_demo


    g++ -std=c++17 -O2 -Wall main.cpp  -o mct8329a_read -li2c



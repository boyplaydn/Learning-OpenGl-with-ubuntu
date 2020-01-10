dau tien dung Qcreate search file shader trong common
tao folder common luu file shader vao do
de load shader dung lenh: g++ -c common/shader.cpp 
de tao file excute  dung lenh: g++ shader.o -o ex2 tutorial02.cpp  -lGL -lGLEW -lglfw -L /usr/local/lib/

cho ex7 SOIL : g++ shader.o controls.o objloader.o texture.o lodepng.o -o ex7 tutorial07.cpp -lSOIL  -lGL -lGLEW -lglfw -L /usr/local/lib/

bien uniform vi no la const ko thay doi trong qua trinh load (dung cho color value)
bien attribute vi no co thay doi trong qua trinh dung (dung cho vertex, element array, ..) vi phai thay doi cac he truc toa do camera


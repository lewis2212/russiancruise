 struct FLAGS
 {
     byte type;
 };
 struct IP
 {
     int ip1;
     int ip2;
     int ip3;
     int ip4;
 };
 struct PORTS
 {
     int port;
 };
 struct list
 {
     byte Size;
     int Hosts;
     byte xz;
     FLAGS flags[16];
     IP ip[16];
     PORTS port[16];
 };

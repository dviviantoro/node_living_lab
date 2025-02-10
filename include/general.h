// indoor
// 10:52:1c:f3:e7:3c
// uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0xF3, 0xE7, 0x3C};

// outdoor
// MAC: 80:7d:3a:76:38:4e
// uint8_t broadcastAddress[] = {0x80, 0x7D, 0x3A, 0x76, 0x38, 0x4E};


// ambient:
// "id_device,float(battery),float(temperature),humidity,light,rain\r\n"
// contoh id_device = "AMB-1"

// surface:
// "id_device,float(battery),float(temperature_top),float(temperature_bot)\r\n"
// contoh id_device = "SUR-1"

// acdc:
// "id_device,volt1,cur1,pwr1,energy1,freq1,pf1,volt2,...sampai 6 sensor,voltDC1,curDC1,voltDC2,...sampai ke 4\r\n"
// contoh id_device = "ACDC-1"
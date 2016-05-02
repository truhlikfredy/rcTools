#define serialSpeed        115200



class PpmPwm {
public:
    PpmPwm(unsigned char ports, ...);
};


void PpmPwm::PpmPwm(unsigned char ports, ...) {
  va_list arguments;
  int port;

  std::cout << "Ports: " << ports << std::endl;

  va_start(arguments, ports);
  for (unsigned char i = 0; i < num; i++) {
    port = va_arg(arguments, int);
    std::cout << port << " ";
  }
  std::cout << std::endl;
  va_end(arguments);
}


void setup() {
  Serial.begin(serialSpeed     );
  PpmPwm ppmPwm(1,2);
}


void loop() {
  Serial.print(" ");
  Serial.println();
  Serial.flush();
}



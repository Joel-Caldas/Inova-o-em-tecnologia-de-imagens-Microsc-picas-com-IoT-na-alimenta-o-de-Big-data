#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Credenciais WiFi
const char *ssid = "FAMILIA CALDAS";
const char *password = "#joelboy11";

// Configurações de IP fixo
IPAddress local_IP(192,168,15,2);  // Defina o IP fixo desejado
IPAddress gateway(192,168,15,1);   // Defina o IP do gateway (geralmente o IP do roteador)
IPAddress subnet(255,255,255,0);   // Máscara de sub-rede (geralmente essa para redes locais)
IPAddress primaryDNS(8,8,8,8);     // DNS primário (Google DNS)
IPAddress secondaryDNS(8,8,4,4);   // DNS secundário (Google DNS)

// Inicializando o servidor na porta 8080
WebServer server(8080);

// Função para streaming de vídeo MJPEG
void handleStream() {
  WiFiClient client = server.client();

  // Cabeçalhos para MJPEG
  String header = "HTTP/1.1 200 OK\r\n";
  header += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(header);

  while (true) {
    camera_fb_t * fb = esp_camera_fb_get(); // Captura o frame da câmera
    if (!fb) {
      Serial.println("Falha ao capturar a imagem");
      continue;
    }

    // Enviar uma parte do MJPEG (cada frame)
    client.write("--frame\r\n");
    client.write("Content-Type: image/jpeg\r\n");

    // Especifica o tamanho do frame
    client.write("Content-Length: ");
    client.write(String(fb->len).c_str());  // Converte para const char*
    client.write("\r\n\r\n");

    // Envia o buffer da imagem
    client.write((const char *)fb->buf, fb->len);
    client.write("\r\n");

    // Libera o frame para reutilização
    esp_camera_fb_return(fb);

    // Verifica se o cliente ainda está conectado
    if (!client.connected()) {
      break;
    }

    delay(100); // Ajuste o intervalo entre frames (pode alterar conforme necessário)
  }
}

// Inicializa a câmera e o servidor
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Configuração da câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000; // Frequência do XCLK ajustada para 20 MHz
  config.frame_size = FRAMESIZE_SVGA;  // Resolução SVGA (800x600)
  config.pixel_format = PIXFORMAT_JPEG;  // Formato JPEG para transmissão
  config.jpeg_quality = 6;  // Qualidade ajustada para 6 (melhor performance)
  config.fb_count = 1;

  // Inicializa a câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Falha na inicialização da câmera com erro 0x%x", err);
    return;
  }

  // Conectar ao WiFi com IP estático
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Falha na configuração do IP estático.");
  }

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");

  // Inicia o servidor de stream
  server.on("/stream", HTTP_GET, handleStream);
  server.begin();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println(":8080/stream' para acessar o stream");
}

void loop() {
  server.handleClient();  // Lidar com as requisições do cliente
  delay(1);
}

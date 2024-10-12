import tkinter as tk
from tkinter import filedialog
from tkinter import messagebox
from PIL import Image, ImageTk
import requests
import os
import io  # Para lidar com os bytes da imagem

class ImageSaverApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Saver")
        
        # Configurações da janela
        self.root.geometry("500x400")
        
        # Frame para o vídeo da ESP32CAM
        self.video_frame = tk.Frame(self.root, width=223, height=145)
        self.video_frame.grid(row=0, column=0, padx=10, pady=10)
        
        self.video_label = tk.Label(self.video_frame)
        self.video_label.pack(fill=tk.BOTH, expand=True)
        
        # Frame para a amostra das imagens
        self.image_box = tk.Frame(self.root, width=234, height=45)
        self.image_box.grid(row=1, column=0, padx=10, pady=10)
        
        self.image_label = tk.Label(self.image_box)
        self.image_label.pack(side=tk.LEFT)

        self.rotation_slider = tk.Scale(self.image_box, from_=0, to=10, orient=tk.HORIZONTAL)
        self.rotation_slider.pack(side=tk.RIGHT)

        self.folder_icon = tk.Button(self.image_box, text="📁", command=self.open_folder)
        self.folder_icon.pack(side=tk.RIGHT)

        # Botão para salvar imagens
        self.save_button = tk.Button(self.root, text="Salvar Imagem", command=self.save_image)
        self.save_button.grid(row=2, column=0, padx=10, pady=10)

        # Inicializa variáveis
        self.saved_images = []
        self.current_image_index = 0
        
        self.update_video_stream()
        
    def update_video_stream(self):
        # Atualiza o frame com o vídeo da ESP32CAM
        try:
            # Substitua pela URL do seu ESP32CAM
            url = "http://192.168.15.5:81/stream"  # Endpoint da imagem da câmera
            img_data = requests.get(url).content
            img = Image.open(io.BytesIO(img_data))
            img = img.resize((223, 145), Image.ANTIALIAS)
            self.video_label.image = ImageTk.PhotoImage(img)
            self.video_label.config(image=self.video_label.image)
        except Exception as e:
            print(f"Erro ao carregar vídeo: {e}")
        
        # Atualiza a imagem a cada 100ms
        self.root.after(100, self.update_video_stream)

    def save_image(self):
        # Salva a imagem atual
        try:
            img_path = filedialog.asksaveasfilename(defaultextension=".jpg",
                                                      filetypes=[("JPEG files", "*.jpg"),
                                                                 ("PNG files", "*.png")])
            if img_path:
                # Salva a imagem no caminho escolhido
                img_data = requests.get("http://192.168.15.5:81/stream").content
                with open(img_path, 'wb') as f:
                    f.write(img_data)
                self.saved_images.append(img_path)
                messagebox.showinfo("Sucesso", "Imagem salva com sucesso!")
        except Exception as e:
            messagebox.showerror("Erro", f"Não foi possível salvar a imagem: {e}")

    def open_folder(self):
        # Abre a pasta onde as imagens estão salvas
        if self.saved_images:
            folder_path = os.path.dirname(self.saved_images[self.current_image_index])
            os.startfile(folder_path)

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageSaverApp(root)
    root.mainloop()

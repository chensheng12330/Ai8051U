import os
import logging
from PIL import Image

# 设置日志
logging.basicConfig(filename="image_compression.log", level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s')

# 定义压缩函数


def compress_image(image_path):
    try:
        # 打开图片
        img = Image.open(image_path)
        # 获取原始图片大小
        original_size = os.path.getsize(image_path)
        # 压缩图像
        img = img.resize(
            (int(img.width * 0.5), int(img.height * 0.5)), Image.Resampling.LANCZOS)
        # 保存为原文件，覆盖原文件
        img.save(image_path)
        # 输出日志
        logging.info(
            f"Compressed: {image_path}, Original size: {original_size / 1024:.2f} KB, Compressed size: {os.path.getsize(image_path) / 1024:.2f} KB")
        print(f"Compressed: {image_path}")
    except Exception as e:
        # 异常处理
        logging.error(f"Error processing {image_path}: {str(e)}")
        print(f"Error processing {image_path}: {str(e)}")

# 遍历当前目录及子目录，扫描图片文件


def scan_and_compress_images(root_folder):
    supported_formats = ['.jpg', '.jpeg', '.png', '.gif', '.bmp']
    for foldername, subfolders, filenames in os.walk(root_folder):
        for filename in filenames:
            # 获取文件扩展名
            file_ext = os.path.splitext(filename)[1].lower()
            if file_ext in supported_formats:
                image_path = os.path.join(foldername, filename)
                compress_image(image_path)


if __name__ == "__main__":
    current_folder = os.getcwd()  # 获取当前工作目录
    scan_and_compress_images(current_folder)

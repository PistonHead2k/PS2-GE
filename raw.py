from PIL import Image
import numpy as np

# Path to your PNG image
png_path = 'bricks128.png'

# Open the PNG image
image = Image.open(png_path)

# Convert the image to a NumPy array
# For a PNG, this will typically be an RGBA array (height, width, 4)
raw_pixel_data = np.asarray(image)

# Now 'raw_pixel_data' is a NumPy array containing the raw pixel values.
# You can then process this array as needed.
print(f"Shape of raw pixel data: {raw_pixel_data.shape}")
print(f"Data type of raw pixel data: {raw_pixel_data.dtype}")

out_file = open("texture.raw", "wb")

out_file.write(raw_pixel_data)
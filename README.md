# C++ Seam Carving Animation

An interactive content-aware image resizing application implemented with openFrameworks that uses the Seam Carving algorithm.

## Overview

Seam Carving is an algorithm for content-aware image resizing that preserves important visual content while resizing images. Unlike traditional scaling methods that might distort important features, Seam Carving intelligently removes pixels of least importance.

This implementation provides:
- Interactive drag-to-resize functionality
- Real-time visual feedback
- Automatic saving of resized images
- Content-aware resizing that preserves important visual elements

## How It Works

The application uses the following steps:
1. **Energy Calculation**: Computes the "energy" of each pixel by measuring color differences with neighboring pixels
2. **Seam Finding**: Uses dynamic programming to find the path of pixels (seam) with minimal visual importance
3. **Seam Removal**: Removes the identified seam and shifts remaining pixels

## Visualization of Seam Carving

### Energy Map
The energy map highlights areas of visual importance:
- Brighter areas indicate higher energy (important visual features)
- Darker areas indicate lower energy (less important for visual content)

### Seam Removal
The algorithm removes seams with the lowest energy, preserving the image's important visual content.

1. Original Image
2. Energy Map Visualization 
3. Identified Seams (shown in red)
4. Result after Seam Removal

This content-aware resizing preserves important structures while reducing image dimensions.

## Features

- **Drag and Drop**: Load images by dragging them into the application window
- **Interactive Resizing**: Resize by dragging the image edges
- **Automatic Centering**: Images are automatically centered in the window
- **Status Feedback**: Visual feedback during operations
- **Result Saving**: Automatically saves resized images as "resized_image.png"

## Requirements

- openFrameworks 0.11.0+
- C++11 compiler

## Usage

1. **Load an image**: Drag and drop an image file into the application window
2. **Resize the image**: Click and drag the image borders to your desired dimensions
3. **Complete resizing**: Release the mouse button to apply Seam Carving and save the result

## Limitations

- Very large images might result in slower performance
- Minimum resize dimensions are restricted (100x100 pixels)

## Implementation Details

The implementation includes several key components:

- `calculateEnergy()`: Calculates image energy based on pixel gradients
- `findVerticalSeam()`: Uses dynamic programming to find minimal energy path
- `removeVerticalSeam()`: Removes selected seam from the image
- `resizeWithSeamCarving()`: Manages the resizing process

## License

MIT

## Credits

Inspired by the original Seam Carving paper:
> Shai Avidan and Ariel Shamir. "Seam Carving for Content-Aware Image Resizing." ACM Transactions on Graphics, Vol. 26, No. 3, Article 10, July 2007.
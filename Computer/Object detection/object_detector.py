import cv2
import numpy as np
from pathlib import Path

class SimpleObjectDetector:
    def __init__(self):
        """Initialize the detector with pre-trained models"""
        print("🚀 Initializing Object Detector...")
        
        # Load pre-trained Haar Cascade models (these come with OpenCV!)
        self.face_cascade = cv2.CascadeClassifier(
            cv2.data.haarcascades + 'haarcascade_frontalface_default.xml'
        )
        self.eye_cascade = cv2.CascadeClassifier(
            cv2.data.haarcascades + 'haarcascade_eye.xml'
        )
        
        print("✅ Detector ready!")
    
    def detect_objects(self, image_path):
        """Detect faces and eyes in an image"""
        print(f"\n📸 Loading image: {image_path}")
        
        # Read the image
        img = cv2.imread(image_path)
        
        if img is None:
            print("❌ Error: Could not load image. Check the file path!")
            return None
        
        # Convert to grayscale (AI models work better with grayscale)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # Detect faces
        print("🔍 Detecting faces...")
        faces = self.face_cascade.detectMultiScale(
            gray, 
            scaleFactor=1.1,  # How much to reduce image size at each scale
            minNeighbors=5,   # How many neighbors each candidate should have
            minSize=(30, 30)  # Minimum object size
        )
        
        print(f"✅ Found {len(faces)} face(s)!")
        
        # Draw rectangles around faces and detect eyes
        for (x, y, w, h) in faces:
            # Draw blue rectangle around face
            cv2.rectangle(img, (x, y), (x+w, y+h), (255, 0, 0), 2)
            cv2.putText(img, 'Face', (x, y-10), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 0), 2)
            
            # Detect eyes within the face region
            roi_gray = gray[y:y+h, x:x+w]
            roi_color = img[y:y+h, x:x+w]
            
            eyes = self.eye_cascade.detectMultiScale(roi_gray)
            
            for (ex, ey, ew, eh) in eyes:
                # Draw green rectangle around eyes
                cv2.rectangle(roi_color, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)
        
        return img
    
    def process_and_save(self, image_path, output_path='output_detected.jpg'):
        """Process image and save result"""
        result = self.detect_objects(image_path)
        
        if result is not None:
            # Save the result
            cv2.imwrite(output_path, result)
            print(f"💾 Result saved to: {output_path}")
            
            # Display the result
            cv2.imshow('Object Detection Result', result)
            print("\n👀 Showing result... Press any key to close the window")
            cv2.waitKey(0)
            cv2.destroyAllWindows()
            
            return True
        return False


def main():
    """Main function to run the detector"""
    print("=" * 60)
    print("   WELCOME TO YOUR OBJECT DETECTION SYSTEM v1.0")
    print("=" * 60)
    
    # Create detector
    detector = SimpleObjectDetector()
    
    # Get image path from user
    print("\n📁 Enter the path to your image file:")
    print("   (Example: C:/Users/YourName/Pictures/photo.jpg)")
    print("   (Or just drag and drop the image file here)")
    image_path = input("\nImage path: ").strip().strip('"').strip("'")
    
    if not Path(image_path).exists():
        print("\n❌ Error: File not found! Please check the path and try again.")
        return
    
    # Process the image
    detector.process_and_save(image_path)
    
    print("\n" + "=" * 60)
    print("   DETECTION COMPLETE! Check output_detected.jpg")
    print("=" * 60)
    print("\n💡 NEXT STEPS:")
    print("   1. Try with different images")
    print("   2. Share any errors you get - we'll fix them together!")
    print("   3. Ready for Phase 2? Tell me and we'll add more features!")


if __name__ == "__main__":
    main()
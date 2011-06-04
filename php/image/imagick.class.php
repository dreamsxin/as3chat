<?php defined('SYSPATH') or die('No direct script access.');
/**
 * Support for image manipulation using [Imagick](http://php.net/Imagick).
 *
 * @package    Kohana/Image
 * @category   Drivers
 * @author     Tamas Mihalik tamas.mihalik@gmail.com
 * @copyright  (c) 2009-2010 Kohana Team
 * @license    http://kohanaphp.com/license.html
 */
class Kohana_Image_Imagick extends Image {

    /**
     * @var  Imagick  image magick object
     */
    protected $_imagick;

    /**
     * Checks if ImageMagick is enabled.
     *
     * @throws  Kohana_Exception
     * @return  boolean
     */
    public static function check() {
//        if (!extension_loaded('imagick')) {
//            throw new Kohana_Exception('Imagick is not installed, or the extension is not loaded');
//        }
        
        if (!class_exists('Imagick')) {
            throw new Exception('ImageImagick requires PECL Imagick. Please see http://pecl.php.net/package/imagick for more information.');
        }

        return self::$_checked = TRUE;
    }

    /**
     * Runs [Image_Imagick::check] and loads the image.
     *
     * @return  void
     * @throws  Kohana_Exception
     */
    public function __construct($file) {
        if (!self::$_checked) {
            self::check();
        }

        parent::__construct($file);

        $this->_imagick = new Imagick;
        if (!$this->_imagick->readImage($this->file)) {
            throw new Exception('Imagick::readImage ' . $this->file . ' failed ');
        }
        //$this->_imagick = $this->_imagick->coalesceImages();
    }

    /**
     * Destroys the loaded image to free up resources.
     *
     * @return  void
     */
    public function __destruct() {
        $this->_imagick->clear();
        $this->_imagick->destroy();
    }

    protected function _do_resize($width, $height) {
        $this->_imagick->setIteratorIndex(0);
        do {
            $this->_imagick->thumbnailImage($width, $height, true);
        } while ($this->_imagick->nextImage());

        $this->width = $this->_imagick->getImageWidth();
        $this->height = $this->_imagick->getImageHeight();
        return TRUE;
    }

    protected function _do_crop($width, $height, $offset_x, $offset_y) {
        $this->_imagick->setIteratorIndex(0);
        do {
            $this->_imagick->cropImage($width, $height, $offset_x, $offset_y);
        } while ($this->_imagick->nextImage());

        $this->width = $this->_imagick->getImageWidth();
        $this->height = $this->_imagick->getImageHeight();
        return TRUE;
    }

    protected function _do_rotate($degrees) {
        $this->_imagick->setIteratorIndex(0);
        try {
            $pixel = new ImagickPixel();
            do {
                $this->_imagick->rotateImage($pixel, $degrees);
            } while ($this->_imagick->nextImage());

            $this->width = $this->_imagick->getImageWidth();
            $this->height = $this->_imagick->getImageHeight();
        } catch (Exception $e) {
            return FALSE;
        }
        return TRUE;
    }

    protected function _do_flip($direction) {
        $func = 'flipImage';
        if ($direction === parent::HORIZONTAL) {
            $func = 'flopImage';
        }
        $this->_imagick->setIteratorIndex(0);
        do {
            $this->_imagick->$func();
        } while ($this->_imagick->nextImage());

        $this->width = $this->_imagick->getImageWidth();
        $this->height = $this->_imagick->getImageHeight();
        return TRUE;
    }

    protected function _do_watermark(Image $image, $offset_x, $offset_y, $opacity) {
        $opacity = $opacity / 100;
        $watermark = new Imagick();
        $watermark->readImageBlob($image->render());
        if ($opacity != NULL) {
            $watermark->setImageOpacity($opacity);
        }

        $this->_imagick->setIteratorIndex(0);
        do {
            //$this->_imagick->setImageMatte(1);
            $this->_imagick->compositeImage($watermark, Imagick::COMPOSITE_OVER, $offset_x, $offset_y);
        } while ($this->_imagick->nextImage());

        $watermark->clear();
        $watermark->destroy();

        return TRUE;
    }

    protected function _do_canvas($width, $height, $offset_x, $offset_y) {
        $color = new ImagickPixel("white");
        $color->setColor("white");

        $canvas = new Imagick();
        $this->_imagick->setIteratorIndex(0);
        do {
            $canvas->newImage($width, $height, $color);
            $canvas->setImageOpacity(0);
            $canvas->compositeImage($this->_imagick, Imagick::COMPOSITE_OVER, $offset_x, $offset_y);
        } while ($this->_imagick->nextImage());

        $this->_imagick->clear();
        $this->_imagick->destroy();
        $this->_imagick = $canvas;
        
        $this->width = $this->_imagick->getImageWidth();
        $this->height = $this->_imagick->getImageHeight();
        return TRUE;
    }

    protected function _do_background($r, $g, $b, $opacity) {
        $opacity = $opacity / 100;
        $color = $r.$g.$b;

        $background = new Imagick();
        $background->newImage($this->_imagick->getImageWidth(), $this->_imagick->getImageHeight(), new ImagickPixel($color));
        $background->setImageOpacity($opacity);
        $this->_imagick->setIteratorIndex(0);
        do {
            $this->_imagick->compositeImage($background, Imagick::COMPOSITE_OVER, 0, 0);
        } while ($this->_imagick->nextImage());

        $background->clear();
        $background->destroy();

        return TRUE;
    }

    protected function _do_mask(proImage $image) {
        $mask = new Imagick();
        $mask->readImageBlob($image->render());

        $this->_imagick->setIteratorIndex(0);
        do {
            $this->_imagick->setImageMatte(1);
            $this->_imagick->compositeImage($mask, Imagick::COMPOSITE_DSTIN, 0, 0);
        } while ($this->_imagick->nextImage());

        $mask->clear();
        $mask->destroy();

        return TRUE;
    }

   protected function _do_sharpen($amount) {
        //IM not support $amount under 5 (0.15)
        $amount = ($amount < 5) ? 5 : $amount;

        // Amount should be in the range of 0.0 to 3.0
        $amount = ($amount * 3.0) / 100;

        return $this->_imagick->sharpenImage(0, $amount);
    }

    protected function _do_reflection($height, $opacity, $fade_in) {
        // Clone the current image and flip it for reflection
        $reflection = $this->_imagick->clone();
        $reflection->flipImage();

        // Crop the reflection to the selected height
        $reflection->cropImage($this->width, $height, 0, 0);
        $reflection->setImagePage($this->width, $height, 0, 0);

        // Select the fade direction
        $direction = array('transparent', 'black');

        if ($fade_in) {
            // Change the direction of the fade
            $direction = array_reverse($direction);
        }

        // Create a gradient for fading
        $fade = new Imagick;
        $fade->newPseudoImage($reflection->getImageWidth(), $reflection->getImageHeight(), vsprintf('gradient:%s-%s', $direction));

        // Apply the fade alpha channel to the reflection
        $reflection->compositeImage($fade, Imagick::COMPOSITE_DSTOUT, 0, 0);

        // NOTE: Using setImageOpacity will destroy alpha channels!
        $reflection->evaluateImage(Imagick::EVALUATE_MULTIPLY, $opacity / 100, Imagick::CHANNEL_ALPHA);

        // Create a new container to hold the image and reflection
        $image = new Imagick;
        $image->newImage($this->width, $this->height + $height, new ImagickPixel);

        // Force the image to have an alpha channel
        $image->setImageAlphaChannel(Imagick::ALPHACHANNEL_SET);

        // Force the background color to be transparent
        // $image->setImageBackgroundColor(new ImagickPixel('transparent'));
        // Match the colorspace between the two images before compositing
        $image->setColorspace($this->_imagick->getColorspace());

        // Place the image and reflection into the container
        if ($image->compositeImage($this->_imagick, Imagick::COMPOSITE_SRC, 0, 0)
                AND $image->compositeImage($reflection, Imagick::COMPOSITE_OVER, 0, $this->height)) {
            // Replace the current image with the reflected image
            $this->_imagick = $image;

            // Reset the width and height
            $this->width = $this->_imagick->getImageWidth();
            $this->height = $this->_imagick->getImageHeight();

            return TRUE;
        }

        return FALSE;
    }

    protected function _do_save($file, $quality) {
        $extension = pathinfo($file, PATHINFO_EXTENSION);
        $this->_imagick->setImageFormat($extension);
        $this->_imagick->setImageCompressionQuality($quality);
        $this->_imagick->optimizeImageLayers();

//        if ($this->_imagick->getNumberImages() > 1 && !in_array(strtolower($extension), array('gif'))) {
//            $this->_imagick->setIteratorIndex(0);
//            return $this->_imagick->writeImage($file);
//        }
        return $this->_imagick->writeImages($file, true);
    }

    protected function _do_render($extension, $quality) {
        if ($extension) {
            $this->_imagick->setImageFormat($extension);
        }

        $this->_imagick->setImageCompressionQuality($quality);
        $this->_imagick->optimizeImageLayers();
//        if ($this->_imagick->getNumberImages() > 1 && !in_array(strtolower($extension), array('gif'))) {
//            $this->_imagick->setIteratorIndex(0);
//            return $this->_imagick->getImageBlob();
//        }
        return $this->_imagick->getImagesBlob();
    }
}

// End Kohana_Image_Imagick

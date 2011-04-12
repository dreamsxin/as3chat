<?php

/**
 * 图像处理支持 - ImageMagick
 * 需要 PECL Imagick 扩展支持
 * @author	Zhuzx
 * @email dreamsxin@qq.com
 * @date	2010-12-30
 */
class Imagick extends Image {

    protected static $_checked;
    protected $_imagick;

    public static function check() {
        if (!class_exists('Imagick')) {
            throw new Exception('ImageImagick requires PECL Imagick. Please see http://pecl.php.net/package/imagick for more information.');
        }
        self::$_checked = TRUE;
    }

    public function __construct($file) {
        if (!self::$_checked) {
            self::check();
        }

        parent::__construct($file);

        $this->_imagick = new Imagick();
        if (!$this->_imagick->readImage($this->file)) {
            throw new Exception('Imagick::readImage '.$this->file.' failed ');
        }
        // $format = strtolower($this->_imagick->getImageFormat());
        $this->_imagick = $this->_imagick->coalesceImages();
    }

    /**
     * 释放资源
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
//		$i = 0;
//		foreach ($this->_imagick as $frame) {
//			echo 'aaa'.($i++);
//			// $this->_imagick->thumbnailImage($width, $height, Imagick::FILTER_LANCZOS, 1);
//			$frame->thumbnailImage($width, $height, true);
//			// $frame->setImagePage($width, $height, 0, 0);
//		}
//		$this->_imagick->coalesceImages();
//		$this->_imagick->thumbnailImage($width, $height);
//		$this->_imagick->resizeImage($width, $height, Imagick::FILTER_CATROM, 1, true);
//		$this->_imagick->resizeImage($width, $height, Imagick::FILTER_LANCZOS, 1);

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

    protected function _do_watermark(proImage $image, $offset_x, $offset_y, $opacity) {
        $watermark = new Imagick();
        $watermark->readImageBlob($image->render());
        $watermark->setImageOpacity($opacity);
        $this->_imagick->setIteratorIndex(0);
        do {
            $this->_imagick->compositeImage($watermark, Imagick::COMPOSITE_OVER, $offset_x, $offset_y);
        } while ($this->_imagick->nextImage());

        $watermark->clear();
        $watermark->destroy();

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

    protected function _do_save($file, $quality) {
        $extension = pathinfo($file, PATHINFO_EXTENSION);
        $this->_imagick->setImageFormat($extension);
        $this->_imagick->setImageCompressionQuality($quality);
        
        if ($this->_imagick->getNumberImages() > 1 && !in_array(strtolower($extension), array('gif'))) {
            $this->_imagick->setIteratorIndex(0);
            return $this->_imagick->writeImage($file);
        }
        $this->_imagick->optimizeImageLayers();
        return $this->_imagick->writeImages($file, true);
    }

    protected function _do_render($extension, $quality) {
        if ($extension) {
            $this->_imagick->setImageFormat($extension);
        }

        $this->_imagick->setImageCompressionQuality($quality);
        $this->_imagick->optimizeImageLayers();
        if ($this->_imagick->getNumberImages() > 1 && !in_array(strtolower($extension), array('gif'))) {
            $this->_imagick->setIteratorIndex(0);
            return $this->_imagick->getImageBlob();
        }
        return $this->_imagick->getImagesBlob();
    }

}
?>

<?php

/**
 * 图像处理 - 进行图片缩放,裁切
 * @author	Zhuzx
 * @email dreamsxin@qq.com
 * @date	2010-12-29
 */
abstract class Image {
    // 缩放模式
    const NONE = 0x01; // 不按照比例
    const WIDTH = 0x02; // 按照比例，以宽度为准
    const HEIGHT = 0x03; // 按照比例，以高度为准
    const AUTO = 0x04; // 自动选择：($this->width / $width) > ($this->height / $height) ? self::WIDTH : self::HEIGHT;
    const INVERSE = 0x05; // 自动选择，与self::AUTO相反
    // 翻转方向
    const HORIZONTAL = 0x11;
    const VERTICAL = 0x12;

    // 默认驱动: GD, Imagick, etc
    public static $default_driver = 'Imagick';

    /**
     * 加载图片，准备处理
     *
     *     $image = self::factory('upload/test.jpg');
     *
     * @param	string	图片路径
     * @param	string	驱动: GD, Imagick, ImageMagick, etc
     * @return	Image
     */
    public static function factory($file, $driver = NULL) {
        if ($driver === NULL) {
            $driver = sfConfig::get('app_image_driver', self::$default_driver);
        }

        $class = 'pro'.ucfirst($driver).'Image';

        return new $class($file);
    }

    public $file;
    public $width;
    public $height;
    // 图像类型，常量值:IMAGETYPE_*
    public $type;

    /**
     * 加载图片，获取基本信息
     *
     * @param	string	图片路径
     * @return	void
     */
    public function __construct($file) {
        $info = $this->get_info($file);

        $this->file = $file;
        $this->width = $info->width;
        $this->height = $info->height;
        $this->type = $info->type;
        $this->mime = $info->mime;
    }

    /**
     * 获取图片信息
     *
     * @param   string    path to file
     * @return  object  file info
     */
    private function get_info($file=NULL) {
        if (!function_exists('getimagesize')) {
            throw new Exception('The Image library requires the getimagesize() PHP function, which is not available in your installation.');
        }

        if ($file === NULL) {
            $file = $this->file;
        }

        try {
            $file = realpath($file);
            $info = getimagesize($file);
        } catch (Exception $e) {
            // echo $e->getMessage();
        }

        if (empty($file) OR empty($info)) {
            throw new Exception('Not an image or invalid image: '.$file);
        }

        $return = new stdClass();

        $return->file = $file;
        $return->width = $info[0];
        $return->height = $info[1];
        $return->type = $info[2];
        $return->mime = image_type_to_mime_type($return->type);

        return $return;
    }

    /**
     * 显示图片
     *
     *     header('Content-Type: image/png');
     *     echo $image;
     *
     * @return  string
     */
    public function __toString() {
        try {
            return $this->render();
        } catch (Exception $e) {
            return '';
        }
    }

    /**
     * 缩放图片，
     *
     *     $image->resize(200, 200);
     *
     * @param   integer  新的宽度
     * @param   integer  新的高度
     * @param   integer  缩放模式:self::AUTO, self::WIDTH, self::HEIGHT, self::NONE
     * @return  $this
     */
    public function resize($width = NULL, $height = NULL, $master = NULL, $zoom = FALSE) {
        if (!$zoom) {
            $thanw = $width / $this->width;
            $thanh = $height / $this->height;
           if ($width && (int)$width > $this->width) {               
               $width = $this->width;
           } elseif ($height && (int)$height > $this->height) {
               $master = $this->width > $this->height ? self::WIDTH : self::HEIGHT;
               $height = $this->height;
           }
        }

        if ($master === NULL) {
            $master = self::AUTO;
        } elseif ($master == self::WIDTH && !empty($width)) {
            $master = self::AUTO;
            $height = NULL;
        } elseif ($master == self::HEIGHT && !empty($height)) {
            $master = self::AUTO;
            $width = NULL;
        }

        if (empty($width)) {
            if ($master === self::NONE) {
                $width = $this->width;
            } else {
                $master = self::HEIGHT;
            }
        }

        if (empty($height)) {
            if ($master === self::NONE) {
                $height = $this->height;
            } else {
                $master = self::WIDTH;
            }
        }

        switch ($master) {
            case self::AUTO:
                $master = ($this->width / $width) > ($this->height / $height) ? self::WIDTH : self::HEIGHT;
                break;
            case self::INVERSE:
                $master = ($this->width / $width) > ($this->height / $height) ? self::HEIGHT : self::WIDTH;
                break;
        }

        switch ($master) {
            case self::WIDTH:
                $height = $this->height * $width / $this->width;
                break;
            case self::HEIGHT:
                $width = $this->width * $height / $this->height;
                break;
        }

        $width = max(round($width), 1);
        $height = max(round($height), 1);

        $this->_do_resize($width, $height);

        return $this;
    }

    /**
     * 剪切图片，未指定偏移量，从中间开始剪切
     *
     *     $image->crop(200, 200);
     *
     * @param   integer  要剪切的宽度
     * @param   integer  要剪切的高度
     * @param   mixed    x轴偏移量
     * @param   mixed    y轴偏移量
     * @return  $this
     */
    public function crop($width, $height, $offset_x = NULL, $offset_y = NULL) {
        if ($width > $this->width) {
            $width = $this->width;
        }

        if ($height > $this->height) {
            $height = $this->height;
        }

        if ($offset_x === NULL) {
            $offset_x = round(($this->width - $width) / 2);
        } elseif ($offset_x === TRUE) {
            $offset_x = $this->width - $width;
        } elseif ($offset_x < 0) {
            $offset_x = $this->width - $width + $offset_x;
        }

        if ($offset_y === NULL) {
            $offset_y = round(($this->height - $height) / 2);
        } elseif ($offset_y === TRUE) {
            $offset_y = $this->height - $height;
        } elseif ($offset_y < 0) {
            $offset_y = $this->height - $height + $offset_y;
        }

        //最大宽度和最大宽度
        $max_width = $this->width - $offset_x;
        $max_height = $this->height - $offset_y;

        if ($width > $max_width) {
            $width = $max_width;
        }

        if ($height > $max_height) {
            $height = $max_height;
        }

        $this->_do_crop($width, $height, $offset_x, $offset_y);

        return $this;
    }

    /**
     * 旋转图片
     *
     *     $image->rotate(90);
     *
     * @param   integer   寻转角度: [-360, 360]
     * @return  $this
     */
    public function rotate($degrees) {
        $degrees = (int) $degrees;

        while ($degrees > 180) {
            $degrees -= 360;
        }

        while ($degrees < -180) {
            $degrees += 360;
        }

        $this->_do_rotate($degrees);

        return $this;
    }

    /**
     * 翻转：垂直、水平
     *
     *     $image->flip(self::HORIZONTAL);
     *
     * @param   integer  翻转模式: self::HORIZONTAL, self::VERTICAL
     * @return  $this
     */
    public function flip($direction) {
        if ($direction !== self::HORIZONTAL) {
            $direction = self::VERTICAL;
        }

        $this->_do_flip($direction);

        return $this;
    }

    /**
     * 增加水印
     *
     *     $mark = self::factory('upload/watermark.png');
     *     $image->watermark($mark, TRUE, TRUE);
     *
     * @param   object   水印图片对象
     * @param   integer  x轴位置
     * @param   integer  y轴位置
     * @param   integer  透明度
     * @return  $this
     */
    public function watermark(Image $watermark, $offset_x = NULL, $offset_y = NULL, $opacity = 100) {
        if ($offset_x === NULL) {
            $offset_x = round(($this->width - $watermark->width) / 2);
        } elseif ($offset_x === TRUE) {
            $offset_x = $this->width - $watermark->width;
        } elseif ($offset_x < 0) {
            $offset_x = $this->width - $watermark->width + $offset_x;
        }

        if ($offset_y === NULL) {
            $offset_y = round(($this->height - $watermark->height) / 2);
        } elseif ($offset_y === TRUE) {
            $offset_y = $this->height - $watermark->height;
        } elseif ($offset_y < 0) {
            $offset_y = $this->height - $watermark->height + $offset_y;
        }

        $opacity = min(max($opacity, 1), 100);

        $this->_do_watermark($watermark, $offset_x, $offset_y, $opacity);

        return $this;
    }

    /**
     * 设置背景色
     *
     *     $image->background('#000');
     *
     * @param   string   颜色值:十六进制
     * @param   integer  透明度: [0, 100]
     * @return  $this
     */
    public function background($color, $opacity = 100) {
        if ($color[0] === '#') {
            $color = substr($color, 1);
        } elseif (strtolower(substr($color, 0, 2)) == '0x') {
            $color = substr($color, 2);
        }

        // 填补到六位
        $color = str_pad($color, 6, $color);

        list ($r, $g, $b) = array_map('hexdec', str_split($color, 2));

        $opacity = min(max($opacity, 0), 100);

        $this->_do_background($r, $g, $b, $opacity);

        return $this;
    }

    /**
     * 遮罩
     *     $mask = self::factory('upload/mask.png');
     *     $data = $image->mask($mask);
     *
     * @param   Oject   遮照图片对象
     * @return  $this
     */
    public function mask(Image $mask) {
	
        $this->_do_mask($mask);

        return $this;
    }

    /**
     * 保存图片
     *
     *     $image->save('saved/cool.png');
     *
     * @param   string   图片保存路径
     * @param   integer  质量: [0, 100]
     * @return  boolean
     */
    public function save($file = NULL, $quality = 100) {
        if ($file === NULL) {
            $file = $this->file;
        }

        if (is_file($file)) {
            if (!is_writable($file)) {
                throw new Exception('File must be writable: '.$file);
            }
        } else {
            $directory = realpath(pathinfo($file, PATHINFO_DIRNAME));

            if (!is_dir($directory) || !is_writable($directory)) {
                throw new Exception('Directory must be writable: '.$directory);
            }
        }

        $quality = min(max($quality, 1), 100);

        return $this->_do_save($file, $quality);
    }

    /**
     * 渲染图片返回图片数据
     *
     *     $data = $image->render('png');
     *
     * @param   string   图片类型: png, jpg, gif, etc
     * @param   integer  质量: [0, 100]
     * @return  string
     */
    public function render($type = NULL, $quality = 100) {
        if ($type === NULL) {
            $type = image_type_to_extension($this->type, FALSE);
        }

        return $this->_do_render($type, $quality);
    }

    /**
     * 缩放抽象方法
     *
     * @param   integer  新的宽度
     * @param   integer  新的高度
     * @return  void
     */
    abstract protected function _do_resize($width, $height);

    /**
     * 剪切抽象方法
     *
     * @param   integer  新的宽度
     * @param   integer  新的高度
     * @param   integer  x轴偏移量
     * @param   integer  y轴偏移量
     * @return  void
     */
    abstract protected function _do_crop($width, $height, $offset_x, $offset_y);

    /**
     * 旋转抽象方法
     *
     * @param   integer  角度
     * @return  void
     */
    abstract protected function _do_rotate($degrees);

    /**
     * 翻转抽象方法
     *
     * @param   integer  方向
     * @return  void
     */
    abstract protected function _do_flip($direction);

    /**
     * 设置水印抽象方法
     *
     * @param   object   图片对象
     * @param   integer  x轴偏移量
     * @param   integer  y轴偏移量
     * @param   integer  透明度
     * @return  void
     */
    abstract protected function _do_watermark(Image $image, $offset_x, $offset_y, $opacity);

    /**
     * 设置背景抽象方法
     *
     * @param   integer  red
     * @param   integer  green
     * @param   integer  blue
     * @param   integer  透明度
     * @return void
     */
    abstract protected function _do_background($r, $g, $b, $opacity);

    /**
     * 遮罩方法
     *
     * @param   object   图片对象
     * @return void
     */
    abstract protected function _do_mask(Image $image);

    /**
     * 保存抽象方法
     *
     * @param   string   新图片地址
     * @param   integer  质量
     * @return  boolean
     */
    abstract protected function _do_save($file, $quality);

    /**
     * 渲染输出图片数据抽象方法
     *
     * @param   string    类型: png, jpg, gif, etc
     * @param   integer   质量
     * @return  string
     */
    abstract protected function _do_render($type, $quality);
}

// End Image

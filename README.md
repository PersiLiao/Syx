# Syx - base of Yaf 3.0.4 

## Requirement
- PHP 7.0 +

## Install
### Compile Syx in Linux
```
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```

## Tutorial

### layout
A classic Application directory layout:

```
- .htaccess // Rewrite rules
+ public
  | - index.php // Application entry
  | + css
  | + js
  | + img
+ conf
  | - application.ini // Configure
- application/
  - Bootstrap.php // Bootstrap
  + index // index module
    + controller
      - Index.php // Default controller
    + model
      - User.php // Model
    + view
      |+ index
          - index.html // View template for default controller
    + plugin
      - System.php
+ library
```

### DocumentRoot
you should set DocumentRoot to application/public, thus only the public folder can be accessed by user

### index.php
index.php in the public directory is the only way in of the application, you should rewrite all request to it(you can use .htaccess in Apache+php mod)

```php
<?php

define("APPLICATION_PATH",  dirname(dirname(__FILE__)));

$app  = new Syx\Application(APPLICATION_PATH . "/conf/application.ini");
$app->bootstrap() //call bootstrap methods defined in Bootstrap.php
    ->run();
```
### Rewrite rules

#### Apache

```conf
#.htaccess
RewriteEngine On
RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule .* index.php
```

#### Nginx

```
server {
  listen ****;
  server_name  domain.com;
  root   document_root;
  index  index.php index.html index.htm;

  if (!-e $request_filename) {
    rewrite ^/(.*)  /index.php/$1 last;
  }
}
```

#### Lighttpd

```
$HTTP["host"] =~ "(www.)?domain.com$" {
  url.rewrite = (
     "^/(.+)/?$"  => "/index.php/$1",
  )
}
```

### application.ini
application.ini is the application config file

```ini
[product]
;CONSTANTS is supported
application.directory = APPLICATION_PATH "/application/"
```
alternatively, you can use a PHP array instead:

```php
<?php
$config = array(
   "application" => array(
       "directory" => application_path . "/application/",
       "namespace" => 'app'  // application default namespace
    ),
);

$app  = new Syx\Application($config);
....

```
### default controller
In Syx, the default controller is named Index:

```php
<?php
namespace app\index\controller;

class Index extends Syx\ControllerAbstract {
   // default action name
   public function indexAction() {
        $this->getView()->content = "Hello World";
   }
}

```

###view script
The view script for default controller and default action is in the application/index/view/index/index.html, Syx provides a simple view engineer called "Syx\View\Simple", which supported the view template written by PHP.

```php
<html>
 <head>
   <title>Hello World</title>
 </head>
 <body>
   <?php echo $content; ?>
 </body>
</html>
```

## Run the Applicatioin
  http://www.yourhostname.com/

## Todo list
1.Single module
2.Integrated swoole


## QQ Group
QQ Group: 545348293

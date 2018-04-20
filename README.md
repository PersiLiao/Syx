![logo](https://avatars3.githubusercontent.com/u/37900031?s=200&v=4)

## 特性

* 超高性能: 基于PHP7+、Swoole, C扩展框架，超高性能
* 支持CLI, CGI(FPM), 常驻内存(Swoole)，多种运行模式
* TCP、UDP、HTTP、WebSocket服务器
* MVC分层设计
* 高性能、可扩展路由，支持自定义路由
* Bootstap、Hook 机制
* 全局异常处理
* 异步Task
* 自定义用户进程
* RPC服务
* 代码热更新

## 环境要求
* PHP 7.0 +
* Swoole 1.10.3 +

## 安装Swoole、Syx

* 下载Swoole、Syx扩展安装包，然后解压，执行以下命令安装
```
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```
* 编辑php.ini
```
[swoole]
extension=swoole.so

[syx]
extension=syx.so
```

## DEMO

### 推荐应用代码结构

```
+ public
  | - syx
+ conf
  | - application.ini // 应用配置文件
- application/
  - Bootstrap.php // Bootstrap
  + index // index模块，默认为index
    + controller
      - Index.php // Index控制器，默认为Index
+ library // 应用类库目录
```

### syx文件内容

```php
!# /usr/bin/env php
<?php

define("APP_PATH",  dirname(__DIR__));

$serv  = new Syx\Server\Http(APP_PATH . "/conf/application.ini");
$serv->bootstrap()->start();
```

### application.ini

```ini
[product]
;ini配置支持PHP常量
application.directory = APP_PATH "/application/"
```

### Index.php

```php
<?php
namespace app\index\controller;

class Index extends Syx\ControllerAbstract {
   // 默认Action
   public function indexAction() {
        // 关闭视图，后期会移除视图
        Dispatcher::getInstance()->disableView();
        $this->getResponse()->setBody('hello world');
   }
}
```

## 特别鸣谢

* 感谢[鸟哥](https://github.com/laruence)为PHP做出的贡献，以及yaf框架代码
* 感谢[Swoole](https://github.com/swoole)开发组提供的Swoole通信引擎
* 感谢[@韩天峰(Rango)](https://github.com/matyhtf)在开发过程中的指导与支持
* 感谢[@盘古大叔(pangudashu)](https://github.com/pangudashu/)在开发过程中的帮助

## QQ群: 545348293

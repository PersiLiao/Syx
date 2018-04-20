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
[common]
; 应用根目录
application.directory = APP_PATH'/application'
; 自动加载目录
application.library = APP_PATH'/library'

; 监听IP
server.host=127.0.0.1
; 监听端口
server.port=5566
; Swoole运行模式
server.mode=SWOOLE_PROCESS
; 使用协议，支持|, SWOOLE_SOCK_TCP | SWOOLE_SSL
server.protocol=SWOOLE_SOCK_TCP
; set 为swoole server 配置选项
; 后台运行
server.set.daemonize=false
; Worker进程数量配置
server.set.worker_num=4
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

* 感谢[@鸟哥(laruence)](https://github.com/laruence)为PHP做出的贡献，以及yaf框架代码
* 感谢[@Swoole开发组](https://github.com/swoole)提供的Swoole通信引擎
* 感谢[@韩天峰(Rango)](https://github.com/matyhtf)在开发过程中的指导与支持
* 感谢[@盘古大叔(pangudashu)](https://github.com/pangudashu/)在开发过程中的帮助

## QQ群: 545348293

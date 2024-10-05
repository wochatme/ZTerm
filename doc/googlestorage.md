# Google Cloud Storage快速上手札记

Google Cloud Stroage是一种云端存储服务。如果我们有Google Cloud的账号，我们可以把本地的文件上传到Google Cloud Storage中的bucket中。 本文档论述了如何快速使用C++开发一个文件上传程序。

我们希望所有的库都是静态库，这样我们最终产生的程序只有一个exe文件，不用带一大堆的.dll文件。 我们希望Debug版本使用/MTd选项，Release版本使用/MT选项。 我们只考虑X64模式的编译，不考虑32位的编译。

你必须先安装完如下软件：
- Visual Studio 2022社区版 (免费)
- CMake最新版(免费)

下文假设上述软件已经安装好了。 下面的所有操作都是通过"x64 Native Tools Command Prompt for VS 2022"窗口进行的。

## 下载安装VCPKG
VCPKG是微软出品的包管理工具，非常好用。它的安装也非常快速。 首先通过git把VCPKG的代码拉到本地。我假设vcpkg下载到c:\windev\vcpkg目录下了。
```
git clone https://github.com/microsoft/vcpkg.git
```

在c:\windev\vcpkg目录下有一个批处理：bootstrap-vcpkg.bat。 执行这个bootstrap-vcpkg.bat，它实际上是从官网下载vcpkg.exe。 有了vcpkg.exe我们就可以下载所需要的Google Cloud CPP客户端的包了。 在c:\windev\vcpkg中执行如下命令：
```
vcpkg.exe search google-cloud-cpp
```
它会列出Google Cloud C++客户端的库的最新版的情况。 然后在此目录下执行下载包的命令：
```
vcpkg.exe install google-cloud-cpp:x64-windows-static
```
接下来就是漫长的等待。VCPKG会依次下载依赖的库，并进行编译。编译的速度取决于你机器的配置。你可能会听到风扇的噪音，你也会看到你的CPU长期处于满负荷工作状态。

下载好的包都在c:\windev\vcpkg\packages目录下。每一个包都有头文件include，发布版(release)和调试版(debug)的库，目录非常清爽，很容易理解。 下面是下载的所有的库的列表：
```
C:\windev\vcpkg\packages>dir

 Directory of C:\windev\vcpkg\packages

09/12/2023  09:55 PM    <DIR>          .
09/12/2023  09:55 PM    <DIR>          ..
09/12/2023  09:08 PM    <DIR>          abseil_x64-windows
09/12/2023  09:08 PM    <DIR>          abseil_x64-windows-static
09/12/2023  09:08 PM    <DIR>          c-ares_x64-windows
09/12/2023  09:09 PM    <DIR>          c-ares_x64-windows-static
09/12/2023  09:08 PM    <DIR>          crc32c_x64-windows-static
09/12/2023  09:11 PM    <DIR>          curl_x64-windows-static
09/12/2023  09:07 PM    <DIR>          detect_compiler_x64-windows
09/12/2023  09:08 PM    <DIR>          detect_compiler_x64-windows-static
09/12/2023  09:51 PM    <DIR>          google-cloud-cpp_x64-windows-static
09/12/2023  09:08 PM    <DIR>          grpc_x64-windows
09/12/2023  09:28 PM    <DIR>          grpc_x64-windows-static
09/12/2023  09:31 PM    <DIR>          nlohmann-json_x64-windows-static
09/12/2023  09:08 PM    <DIR>          openssl_x64-windows
09/12/2023  09:08 PM    <DIR>          openssl_x64-windows-static
09/12/2023  09:08 PM    <DIR>          protobuf_x64-windows
09/12/2023  09:13 PM    <DIR>          protobuf_x64-windows-static
09/12/2023  09:08 PM    <DIR>          re2_x64-windows
09/12/2023  09:14 PM    <DIR>          re2_x64-windows-static
09/12/2023  09:08 PM    <DIR>          upb_x64-windows
09/12/2023  09:14 PM    <DIR>          upb_x64-windows-static
09/12/2023  09:08 PM    <DIR>          vcpkg-cmake-config_x64-windows
09/12/2023  09:08 PM    <DIR>          vcpkg-cmake-get-vars_x64-windows
09/12/2023  09:08 PM    <DIR>          vcpkg-cmake_x64-windows
09/12/2023  09:08 PM    <DIR>          zlib_x64-windows
09/12/2023  09:08 PM    <DIR>          zlib_x64-windows-static
               0 File(s)              0 bytes
              27 Dir(s)  233,114,890,240 bytes free

```
我们只挑选后缀名是-static的静态库进行链接即可。 其中abseil_x64-windows-static里面的库有很多小库，链接起来太繁琐。我们可以把它们打包成一个大库。 假设你有两个库: libABC.lib和libXYZ.lib，你可以使用VSTS 2022自带的lib.exe对它们进行打包：
```
lib.exe /OUT:absl_all.lib libABC.lib libXYZ.lib
```

上述命令就把两个小库合并成一个大库absl_all.lib。类似的，你可以把多个库合并成一个库。我采用了这个办法，把所有的abseil的库合并成了一个absl_all.lib
```
lib /out:absl_all.lib absl_bad_any_cast_impl.lib absl_bad_optional_access.lib absl_bad_variant_access.lib absl_base.lib absl_city.lib absl_civil_time.lib absl_cord.lib absl_cord_internal.lib absl_cordz_functions.lib absl_cordz_handle.lib absl_cordz_info.lib absl_cordz_sample_token.lib absl_crc32c.lib absl_crc_cord_state.lib absl_crc_cpu_detect.lib absl_crc_internal.lib absl_debugging_internal.lib absl_demangle_internal.lib absl_die_if_null.lib absl_examine_stack.lib absl_exponential_biased.lib absl_failure_signal_handler.lib absl_flags.lib absl_flags_commandlineflag.lib absl_flags_commandlineflag_internal.lib absl_flags_config.lib absl_flags_internal.lib absl_flags_marshalling.lib absl_flags_parse.lib absl_flags_private_handle_accessor.lib absl_flags_program_name.lib absl_flags_reflection.lib absl_flags_usage.lib absl_flags_usage_internal.lib absl_graphcycles_internal.lib absl_hash.lib absl_hashtablez_sampler.lib absl_int128.lib absl_kernel_timeout_internal.lib absl_leak_check.lib absl_log_entry.lib absl_log_flags.lib absl_log_globals.lib absl_log_initialize.lib absl_log_internal_check_op.lib absl_log_internal_conditions.lib absl_log_internal_format.lib absl_log_internal_globals.lib absl_log_internal_log_sink_set.lib absl_log_internal_message.lib absl_log_internal_nullguard.lib absl_log_internal_proto.lib absl_log_severity.lib absl_log_sink.lib absl_low_level_hash.lib absl_malloc_internal.lib absl_periodic_sampler.lib absl_random_distributions.lib absl_random_internal_distribution_test_util.lib absl_random_internal_platform.lib absl_random_internal_pool_urbg.lib absl_random_internal_randen.lib absl_random_internal_randen_hwaes.lib absl_random_internal_randen_hwaes_impl.lib absl_random_internal_randen_slow.lib absl_random_internal_seed_material.lib absl_random_seed_gen_exception.lib absl_random_seed_sequences.lib absl_raw_hash_set.lib absl_raw_logging_internal.lib absl_scoped_set_env.lib absl_spinlock_wait.lib absl_stacktrace.lib absl_status.lib absl_statusor.lib absl_str_format_internal.lib absl_strerror.lib absl_string_view.lib absl_strings.lib absl_strings_internal.lib absl_symbolize.lib absl_synchronization.lib absl_throw_delegate.lib absl_time.lib absl_time_zone.lib
```

## 编译Hello World程序
这个例子的源码是从谷歌的文档上摘录的：
```
#include "google/cloud/storage/client.h"
#include <iostream>

char context[16] = {0};

int main(int argc, char* argv[]) 
{
  if (argc != 3) {
    std::cerr << "Missing bucket name.\n";
    std::cerr << "Usage: quickstart <bucket-name> filename\n";
    return 1;
  }
  std::string const bucket_name = argv[1];

  // Create aliases to make the code easier to read.
  namespace gcs = ::google::cloud::storage;

  // Create a client to communicate with Google Cloud Storage. This client
  // uses the default configuration for authentication and project id.
  auto client = gcs::Client();

  auto writer = client.WriteObject(bucket_name, argv[2]);
  for(int i=0; i<16; i++) context[i] = 16 - i;
  context[15] = 0;
  //writer << "Hello BeanStalk!";
  writer << context;
  writer.Close();
  if (!writer.metadata()) {
    std::cerr << "Error creating object: " << writer.metadata().status()
              << "\n";
    return 1;
  }
  std::cout << "Successfully created object: " << *writer.metadata() << "\n";
/*
  auto reader = client.ReadObject(bucket_name, argv[2]);
  if (!reader) {
    std::cerr << "Error reading object: " << reader.status() << "\n";
    return 1;
  }

  //std::string contents{std::istreambuf_iterator<char>{reader}, {}};
  //std::cout << contents << "\n";
*/
  return 0;
}

```

假设上述源文件叫beanstalk.cpp。使用如下方法进行编译：
```
cl /EHsc /MT /c /Ic:\windev\vcpkg\packages\google-cloud-cpp_x64-windows-static/include /Ic:\windev\vcpkg\packages\abseil_x64-windows-static\include beanstalk.cpp   
```

编译成功后产生beanstalk.obj文件。 然后使用如下方法进行链接：
```
link beanstalk.obj zlib.lib libcrypto.lib libssl.lib cares.lib libcurl.lib crc32c.lib absl_all.lib google_cloud_cpp_common.lib google_cloud_cpp_rest_internal.lib google_cloud_cpp_storage.lib ws2_32.lib bcrypt.lib kernel32.lib user32.lib gdi32.lib winspool.lib  shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib crypt32.lib Wldap32.lib
```
当然，你最好把c:\windev\vcpkg\packages中相关的库拷贝到一个目录下，这样链接的时候就不用指定冗长的路径了。Release和Debug模式一样，只是指定的库不一样而已。

编译成功后，你可以测试一下产生的beanstalk.exe。前提是你有Google云的账号，然后创建一个bucket叫beanstalk，然后在这个bukcet上创建一个key文件，格式是json的。这些工作请自行查看Google Cloud Console的相关内容。 我们的service account key file叫做：datamon-398617-e76c120f1f0e.json。你需要在同一个窗口中设置一下环境变量，因为这个小程序使用缺省的认证方式。
```
set GOOGLE_APPLICATION_CREDENTIALS=datamon-398617-e76c120f1f0e.json
```

然后执行程序。第一个参数是bucket的名字，第二个参数是上传文件的名字。 里面的内容写死了，为"Hello BeanStalk!"。 执行结果如下：
```
C:\windev\beanstalk\mt>beanstalk.exe beanstalk allgood.txt
Successfully created object: ObjectMetadata={name=allgood.txt, acl=[], bucket=beanstalk, cache_control=, component_count=0, content_disposition=, content_encoding=, content_language=, content_type=, crc32c=Z43uuw==, etag=CPnomoG/p4EDEAE=, event_based_hold=false, generation=1694604850541689, id=beanstalk/allgood.txt/1694604850541689, kind=storage#object, kms_key_name=, md5_hash=A4Vivhb7cK7IZkVpgl4zwQ==, media_link=https://storage.googleapis.com/download/storage/v1/b/beanstalk/o/allgood.txt?generation=1694604850541689&alt=media, , metageneration=1, name=allgood.txt, retention_expiration_time=1970-01-01T00:00:00Z, self_link=https://www.googleapis.com/storage/v1/b/beanstalk/o/allgood.txt, size=16, storage_class=NEARLINE, temporary_hold=false, time_created=16946048505740000, time_deleted=0, time_storage_class_updated=16946048505740000, updated=16946048505740000}
Hello BeanStalk!
```

我们看到了，它可以成功地上传文件，也可以下载文件查看里面的内容。 至此，一个完成的流程就跑通了。


```
#include "google/cloud/storage/client.h"
#include <iostream>

char context[16] = {0};

int main(int argc, char* argv[]) 
{
  if (argc != 3) {
    std::cerr << "Missing bucket name.\n";
    std::cerr << "Usage: quickstart <bucket-name> filename\n";
    return 1;
  }
  std::string const bucket_name = argv[1];

  // Create aliases to make the code easier to read.
  namespace gcs = ::google::cloud::storage;

  auto credentials = google::cloud::storage::oauth2::CreateServiceAccountCredentialsFromJsonFilePath("c:\\windev\\datamon-398617-e76c120f1f0e.json");
  auto cs = credentials.value();
  auto client = gcs::Client(std::move(cs));

  auto writer = client.WriteObject(bucket_name, argv[2]);
  for(int i=0; i<16; i++) context[i] = 16 - i;
  context[15] = 0;
  writer << context;
  writer.Close();
  if (!writer.metadata()) {
    std::cerr << "Error creating object: " << writer.metadata().status()
              << "\n";
    return 1;
  }
  std::cout << "Successfully created object: " << *writer.metadata() << "\n";
/*
  auto reader = client.ReadObject(bucket_name, argv[2]);
  if (!reader) {
    std::cerr << "Error reading object: " << reader.status() << "\n";
    return 1;
  }

  //std::string contents{std::istreambuf_iterator<char>{reader}, {}};
  //std::cout << contents << "\n";
*/
  return 0;
}

```

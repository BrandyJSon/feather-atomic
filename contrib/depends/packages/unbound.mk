package=unbound
$(package)_version=1.19.2
$(package)_download_path=https://www.nlnetlabs.nl/downloads/$(package)/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=cc560d345734226c1b39e71a769797e7fdde2265cbb77ebce542704bba489e55
$(package)_dependencies=openssl expat
$(package)_patches=disable-glibc-reallocarray.patch

define $(package)_set_vars
  $(package)_config_opts=--disable-shared --enable-static --without-pyunbound --prefix=$(host_prefix)
  $(package)_config_opts+=--with-libexpat=$(host_prefix) --with-ssl=$(host_prefix) --with-libevent=no
  $(package)_config_opts+=--without-pythonmodule --disable-flto --with-pthreads --with-libunbound-only
  $(package)_config_opts_linux=--with-pic
  $(package)_config_opts_w64=--enable-static-exe --sysconfdir=/etc --prefix=$(host_prefix) --target=$(host_prefix)
  $(package)_config_opts_x86_64_darwin=ac_cv_func_SHA384_Init=yes
  $(package)_build_opts_mingw32=LDFLAGS="$($(package)_ldflags) -lpthread"
endef

define $(package)_preprocess_cmds
  patch -p1 < $($(package)_patch_dir)/disable-glibc-reallocarray.patch &&\
  autoconf
endef

define $(package)_config_cmds
  $($(package)_autoconf) ac_cv_func_getentropy=no
endef

define $(package)_build_cmds
  $(MAKE) $($(package)_build_opts)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

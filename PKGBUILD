# Maintainer: Baptiste "Saend" Ceillier <https://github.com/saend>

pkgname=cutl
pkgver=2.0.0
pkgrel=1
pkgdesc="A simple C and Lua unit testing library."
arch=('i686' 'x86_64')
url="https://github.com/saend/cutl"
license=('MIT')
depends=('lua>=5.3')
makedepends=(meson)
source=("$pkgname-$pkgver.tar.gz::$url/archive/$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
	cd "$srcdir/$pkgname-$pkgver"
	meson --prefix /usr --buildtype plain build
	ninja -C build
}

package() {
	cd "$srcdir/$pkgname-$pkgver"
	DESTDIR="$pkgdir" ninja -C build install

	# Lua module
	mkdir -p "$pkgdir/usr/lib/lua/5.3/"
	ln -sr "$pkgdir/usr/lib/liblutl.so" "$pkgdir/usr/lib/lua/5.3/lutl.so"
}

check() {
	cd "$srcdir/$pkgname-$pkgver"
	ninja -C build test
}

# Maintainer: Saend <https://github.com/saend>

pkgname=('cutl')
pkgver=1.0.0
pkgrel=1
pkgdesc="A simple C and Lua unit testing library."
arch=('any')
url="https://github.com/saend/cutl"
license=('MIT')
optdepends=('lua: lua support')
source=("$pkgname-$pkgver.tar.gz::$url/archive/$pkgver.tar.gz")
md5sums=('SKIP')

package() {
	cd "$srcdir/$pkgname-$pkgver"
	make DESTDIR="$pkgdir/" install
}

require 'formula'
class GoogleTest < Formula
  homepage 'http://code.google.com/p/googletest/'
  url 'http://googletest.googlecode.com/files/gtest-1.6.0.zip'
  md5 '4577b49f2973c90bf9ba69aa8166b786'

  def install
    system "./configure --prefix=#{prefix}"
    system 'make install'
  end
end

%global commit_simde 29b911059dd06bc1e4c30d8922871a4b239279e3
%global short_commit_simde %(c=%{commit_simde}; echo ${c:0:7})
%global commit_munit da8f73412998e4f1adf1100dc187533a51af77fd
%global hedley_version 12
# Disable debuginfo package for the header only package.
%global debug_package %{nil}

Name: simde
# Use a minimum version as there has not been a version release yet.
# The upstream mentioned "I think I'll start with a 0.5 or something".
# https://github.com/nemequ/simde/issues/50
Version: 0.0.1
# Align the release format with the packages setting Source0 by commit hash
# such as podman.spec and moby-engine.spec.
Release: 1.git%{short_commit_simde}%{?dist}
Summary: SIMD Everywhere
# find simde/ -type f | xargs licensecheck
# simde: MIT
# simde/check.h: CC0
# simde/debug-trap.h: CC0
# simde/hedley.h: CC0
# simde/simde-arch.h: CC0
License: MIT and CC0
URL: https://github.com/nemequ/simde
Source0: https://github.com/nemequ/%{name}/archive/%{commit_simde}.tar.gz
# munit used in the unit test.
Source1: https://github.com/nemequ/munit/archive/%{commit_munit}.tar.gz
# gcc and clang are used in the unit tests.
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: clang
BuildRequires: cmake
# Do not set noarch for header only package.
# See https://docs.fedoraproject.org/en-US/packaging-guidelines/#_packaging_header_only_libraries

# simde/hedley.h
# https://github.com/nemequ/hedley
Provides: bundled(hedley) = %{hedley_version}

%description
%{summary}
The SIMDe header-only library provides fast, portable implementations of SIMD
intrinsics on hardware which doesn't natively support them, such as calling
SSE functions on ARM. There is no performance penalty if the hardware supports
the native implementation (e.g., SSE/AVX runs at full speed on x86,
NEON on ARM, etc.).

%package devel
Summary: Header files for SIMDe development
Provides: %{name}-static = %{version}-%{release}

%description devel
The simde-devel package contains the header files needed
to develop programs that use the SIMDe.

%prep
%setup -q -n %{name}-%{commit_simde}

%build
# The %%build section is not used.

%install
mkdir -p %{buildroot}%{_includedir}
cp -a simde %{buildroot}%{_includedir}

%check
# Check if all the shipped file is a valid header file.
for file in $(find simde/ -type f); do
  if ! [[ "${file}" =~ \.h$ ]]; then
    echo "${file} is not a header file."
    false
  elif [ -x "${file}" ]; then
    echo "${file} has executable bit."
    false
  fi
done

# Check hedley version correctness.
test "$(grep '^#define HEDLEY_VERSION ' simde/hedley.h | cut -d ' ' -f3)" = \
  '%{hedley_version}'

# Set munit.
rm -rf test/munit
tar xzvf %{SOURCE1}
mv munit-%{commit_munit} test/munit

# Run the unit tests.
# gcc
echo "== 1. tests on gcc =="
gcc --version
g++ --version

# without flags
echo "=== 1.1. tests on gcc without flags ==="
mkdir test/build-gcc
pushd test/build-gcc
CC=gcc CXX=g++ cmake ..
%make_build
./run-tests
popd

# with flags
echo "=== 1.2. tests on gcc with flags ==="
mkdir test/build-gcc-with-flags
pushd test/build-gcc-with-flags
CC=gcc CXX=g++ cmake \
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
  -DCMAKE_C_FLAGS="%{build_cflags}" \
  -DCMAKE_CXX_FLAGS="%{build_cxxflags}" \
  ..
%make_build
./run-tests
popd

# clang
echo "== 2. tests on clang =="
clang --version
clang++ --version

# without flags
echo "=== 2.1. tests on clang without flags ==="
mkdir test/build-clang
pushd test/build-clang
CC=clang CXX=clang++ cmake ..
%make_build
./run-tests
popd

# with flags
echo "=== 2.2. tests on clang with flags ==="
# Some flags and specs are not available with clang.
# https://lists.fedoraproject.org/archives/list/packaging@lists.fedoraproject.org/message/W5UFLUADNB4VF3OBUBSNAPOQL6XBCP74/
optflags_clang=$(echo "%{optflags}" | \
  sed -e 's| -fstack-clash-protection||' -e 's| -specs=[^ ]*||g')
mkdir test/build-clang-with-flags
pushd test/build-clang-with-flags
CC=clang CXX=clang++ cmake \
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
  -DCMAKE_C_FLAGS="${optflags_clang}" \
  -DCMAKE_CXX_FLAGS="${optflags_clang}" \
  ..
%make_build
./run-tests
popd

%files
%license COPYING
%doc README.md

%files devel
%{_includedir}/%{name}

%changelog
* Sun Apr 05 2020 Jun Aruga <jaruga@redhat.com> - 0.0.1-1.git9efa34c
- Initial package

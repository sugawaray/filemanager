#! /bin/sh -

prepare_fmdir() {
	rm -fR "${1}"
	mkdir "${1}"
	cd "${1}"
	fm-init
}

new_file() {
	rm -fR "${1}"
	if [ $# -eq 1 ]
	then
		cat <<- EOF > "${1}"
file content
EOF
	else
		cat <<- EOF > "${1}"
${2}
EOF
	fi
}

new_cat_file() {
	if [ $# -ge 3 ]
	then
		new_file "${1}" "${3}"
	else
		new_file "${1}"
	fi
	fm-set "${1}" "${2}"
}

new_dir() {
	rm -fR "${1}"
	mkdir "${1}"
}

file_exists() {
	if ! [ -f "${1}" ]
	then
		echo "file(${1}) does not exist."
	fi
}

file_does_not_exist() {
	if [ -f "${1}" ]
	then
		echo "file(${1}) exists."
	fi
}

dir_does_not_exist() {
	if [ -d "${1}" ]
	then
		echo "dir(${1}) exists."
	fi
}

dir_exists() {
	if ! [ -d "${1}" ]
	then
		echo "dir(${1}) does not exist."
	fi
}

equal_files() {
	cmp "${1}" "${2}"
	if [ $? -ne 0 ]
	then
		echo "different files(${1},${2})."
		return 1
	fi
	return 0
}

equal_content() {
	new_file "${3}" "${1}"
	if ! equal_files "${3}" "${2}"
	then
		echo "different content(${1},${2})."
		return 1
	fi
	return 0
}

exit_status() {
	if [ $? -ne "${1}" ]
	then
		echo "exit status(${2}) is not (${1})."
	fi
}

does_not_have_any_cats() {
	fm-getcat "${1}" | wc -l > ./t1
	rm ./t2
	cat <<- EOF > ./t2
0
EOF
	equal_files ./t1 ./t2
}

have_the_same_cat() {
	if [ "x$(fm-getcat "${1}" | tr -d '\n')" != "x${2}" ]
	then
		echo "have_the_same_cat"
	fi
}

cat_does_not_have_any_values() {
	if [ $(fm-get "${1}" | wc -l) -ne 0 ]
	then
		echo "cat_does_not_have_any_values(${1})"
	fi
}

echo "fm-cp 1"
(
prepare_fmdir d1
new_file srcfile1 "content"
rm -fR destfile1

fm-cp srcfile1 destfile1
exit_status 0 "fm-cp"

file_exists destfile1
equal_files srcfile1 destfile1
)

echo "fm-cp 2"
(
prepare_fmdir d1
new_file srcfile1 "content1"
new_cat_file destfile1 catA "content2"

fm-cp srcfile1 destfile1
exit_status 0 "fm-cp"

equal_files srcfile1 destfile1
does_not_have_any_cats destfile1
)

echo "fm-cp 3"
(
prepare_fmdir d1
new_file srcfile1 "content1"
new_file destfile1 "content2"

fm-cp srcfile1 destfile1
exit_status 0 "fm-cp"

equal_files srcfile1 destfile1
)

echo "fm-cp 4"
(
prepare_fmdir d1
new_cat_file srcfile1 catA "content1"
rm -fR destfile1

fm-cp srcfile1 destfile1
exit_status 0 "fm-cp"

equal_files srcfile1 destfile1
have_the_same_cat destfile1 catA
)

echo "fm-cp 5"
(
prepare_fmdir d1
new_cat_file srcfile1 catA "content1"
new_cat_file destfile1 catB "content2"

fm-cp srcfile1 destfile1
exit_status 0 "fm-cp"

equal_files srcfile1 destfile1
have_the_same_cat destfile1 catA
)

echo "fm-cp 6"
(
prepare_fmdir d1
new_cat_file srcfile1 catA "content1"
new_file destfile1 "content2"

fm-cp srcfile1 destfile1
exit_status 0 "fm-cp"

equal_files srcfile1 destfile1
have_the_same_cat destfile1 catA
)

echo "fm-cp 7"
(
prepare_fmdir d1
new_file srcfile1 "content1"
new_dir destdir1

fm-cp srcfile1 destdir1
exit_status 0 "fm-cp"

equal_files srcfile1 destdir1/srcfile1
)

echo "fm-cp 8"
(
prepare_fmdir d1
new_cat_file srcfile1 catA "content1"
new_dir destdir1

fm-cp srcfile1 destdir1
exit_status 0 "fm-cp"

equal_files srcfile1 destdir1/srcfile1
have_the_same_cat destdir1/srcfile1 catA
)

echo "fm-cp 9"
(
prepare_fmdir d1
new_file srcfile1
rm -fR ../destfile1

fm-cp srcfile1 ../destfile1
exit_status 0 "fm-cp"

file_exists ../destfile1
equal_files srcfile1 ../destfile1

fm-getcat ../destfile1
exit_status 1 "fm-getcat"
)

echo "fm-cp 10"
(
prepare_fmdir d1
new_file srcfile1
new_file ../destfile1

fm-cp srcfile1 ../destfile1
exit_status 0

file_exists ../destfile1
equal_files srcfile1 ../destfile1

fm-getcat ../destfile1
exit_status 1 "fm-getcat"

does_not_have_any_cats ../destfile1
)

echo "fm-cp 11"
(
prepare_fmdir d1
new_file srcfile1
fm-set srcfile1 catA
rm -fR ../destfile1

fm-cp srcfile1 ../destfile1
exit_status 0 "fm-cp"

file_exists ../destfile1
equal_files srcfile1 ../destfile1

fm-getcat ../destfile1
exit_status 1 "fm-getcat"

does_not_have_any_cats ../destfile1
)

echo "fm-cp 12"
(
prepare_fmdir d1
new_file srcfile1
fm-set srcfile1 catA
new_file ../destfile1

fm-cp srcfile1 ../destfile1
exit_status 0 "fm-cp"

file_exists ../destfile1
equal_files srcfile1 ../destfile1

fm-getcat ../destfile1
exit_status 1 "fm-getcat"

does_not_have_any_cats ../destfile1
)

echo "fm-mv 1"
(
prepare_fmdir d1
new_file srcfile1 ""
new_dir destdir1

fm-mv srcfile1 destdir1/destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
file_exists destdir1/destfile1

does_not_have_any_cats destdir1/destfile1
)

echo "fm-mv 2"
(
prepare_fmdir d1
new_file srcfile1 ""
rm -fR ../destfile1

fm-mv srcfile1 ../destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
file_exists ../destfile1

does_not_have_any_cats ../destfile1
)

echo "fm-mv 3"
(
prepare_fmdir d1
new_file srcfile1 ""
new_dir destdir1
new_file destdir1/destfile1 "content"

fm-mv srcfile1 destdir1/destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destdir1/destfile1 tmpfile1

does_not_have_any_cats destdir1/destfile1
)

echo "fm-mv 4"
(
prepare_fmdir d1
new_file srcfile1 ""
new_file ../destfile1 "content"

fm-mv srcfile1 ../destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" ../destfile1 tmpfile1

does_not_have_any_cats ../destfile1
)

echo "fm-mv 6"
(
prepare_fmdir d1
new_file srcfile1 ""
new_file destfile1 "content"
fm-set destfile1 catA

fm-mv srcfile1 destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destfile1 tmpfile1

does_not_have_any_cats destfile1
)

echo "fm-mv 7"
(
prepare_fmdir d1
new_file srcfile1 ""
mkdir destdir1

fm-mv srcfile1 destdir1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destdir1/srcfile1 tmpfile1

does_not_have_any_cats destdir1/srcfile1
)

echo "fm-mv 8"
(
prepare_fmdir d1
new_file srcfile1 ""
mkdir ../destdir1

fm-mv srcfile1 ../destdir1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" ../destdir1/srcfile1 tmpfile1

does_not_have_any_cats ../destdir1/srcfile1
)

echo "fm-mv 9"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
rm -fR destfile1

fm-mv srcfile1 destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destfile1 tmpfile1

have_the_same_cat destfile1 catA
)

echo "fm-mv 10"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
rm -fR ../destfile1

fm-mv srcfile1 ../destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" ../destfile1 tmpfile1

does_not_have_any_cats ../destfile1
)

echo "fm-mv 11"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
new_file destfile1 "content"

fm-mv srcfile1 destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destfile1 tmpfile1

have_the_same_cat destfile1 catA
)

echo "fm-mv 12"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
new_file ../destfile1 "content"

fm-mv srcfile1 ../destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" ../destfile1 tmpfile1

does_not_have_any_cats ../destfile1
)

echo "fm-mv 14"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
new_cat_file destfile1 catB "content"

fm-mv srcfile1 destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destfile1 tmpfile1

have_the_same_cat destfile1 catA
)

echo "fm-mv 15"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
new_file ../destfile1 "content"

fm-mv srcfile1 ../destfile1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" ../destfile1 tmpfile1

does_not_have_any_cats ../destfile1
)

echo "fm-mv 16"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
new_dir destdir1

fm-mv srcfile1 destdir1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" destdir1/srcfile1 tmpfile1

have_the_same_cat destdir1/srcfile1 catA
)

echo "fm-mv 17"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
new_dir ../destdir1

fm-mv srcfile1 ../destdir1
exit_status 0 "fm-mv"

file_does_not_exist srcfile1
equal_content "" ../destdir1/srcfile1 tmpfile1

does_not_have_any_cats ../destdir1/srcfile1
)

echo "fm-mv 18"
(
prepare_fmdir d1
new_dir srcdir1
rm -fR destfile1

fm-mv srcdir1 destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
dir_exists destfile1
)

echo "fm-mv 19"
(
prepare_fmdir d1
new_dir srcdir1
new_file srcdir1/srcfile1
rm -fR destfile1

fm-mv srcdir1 destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists destfile1/srcfile1
)

echo "fm-mv 20"
(
prepare_fmdir d1
new_dir srcdir1
new_cat_file srcdir1/srcfile1 catA ""
rm -fR destfile1

fm-mv srcdir1 destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists destfile1/srcfile1

have_the_same_cat destfile1/srcfile1 catA
)

echo "fm-mv 21"
(
prepare_fmdir d1
new_dir srcdir1
rm -fR ../destfile1

fm-mv srcdir1 ../destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
dir_exists ../destfile1
)

echo "fm-mv 22"
(
prepare_fmdir d1
new_dir srcdir1
new_file srcdir1/srcfile1
rm -fR ../destfile1

fm-mv srcdir1 ../destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists ../destfile1/srcfile1
)

echo "fm-mv 23"
(
prepare_fmdir d1
new_dir srcdir1
new_cat_file srcdir1/srcfile1 catA ""
rm -fR ../destfile1

fm-mv srcdir1 ../destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists ../destfile1/srcfile1

does_not_have_any_cats ../destfile1/srcfile1
)

echo "fm-mv 24"
(
prepare_fmdir d1
new_dir srcdir1
new_dir destdir1

fm-mv srcdir1 destdir1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
dir_exists destdir1/srcdir1
)

echo "fm-mv 25"
(
prepare_fmdir d1
new_dir srcdir1
new_file srcdir1/srcfile1
new_dir destdir1

fm-mv srcdir1 destdir1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists destdir1/srcdir1/srcfile1
)

echo "fm-mv 26"
(
prepare_fmdir d1
new_dir srcdir1
new_cat_file srcdir1/srcfile1 catA ""
new_dir destdir1

fm-mv srcdir1 destdir1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists destdir1/srcdir1/srcfile1

have_the_same_cat destdir1/srcdir1/srcfile1 catA
)

echo "fm-mv 27"
(
prepare_fmdir d1
new_dir srcdir1
new_dir ../destfile1

fm-mv srcdir1 ../destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
dir_exists ../destfile1/srcdir1
)

echo "fm-mv 28"
(
prepare_fmdir d1
new_dir srcdir1
new_file srcdir1/srcfile1
new_dir ../destfile1

fm-mv srcdir1 ../destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists ../destfile1/srcdir1/srcfile1
)

echo "fm-mv 29"
(
prepare_fmdir d1
new_dir srcdir1
new_cat_file srcdir1/srcfile1 catA ""
new_dir ../destfile1

fm-mv srcdir1 ../destfile1
exit_status 0 "fm-mv"

dir_does_not_exist srcdir1
file_exists ../destfile1/srcdir1/srcfile1

does_not_have_any_cats ../destfile1/srcdir1/srcfile1
)

echo "fm-mv 30"
(
prepare_fmdir d1
new_file srcfile1 "contentA"

fm-mv srcfile1 srcfile1
exit_status 0 "fm-mv"

file_exists srcfile1
equal_content "contentA" srcfile1 tmpfile1
)

echo "fm-mv 31"
(
prepare_fmdir d1
new_cat_file srcfile1 "catA" "contentA"

fm-mv srcfile1 srcfile1
exit_status 0 "fm-mv"

file_exists srcfile1
equal_content "contentA" srcfile1 tmpfile1

have_the_same_cat srcfile1 "catA"
)

echo "fm-refresh 1"
(
prepare_fmdir d1
new_cat_file srcfile1 catA ""
rm srcfile1

fm-refresh

cat_does_not_have_any_values catA
)

echo "fm-rm 1"
(
prepare_fmdir d1
rm -fR targetfile1

fm-rm targetfile1
exit_status 0 "fm-rm"
)

echo "fm-rm 2"
(
prepare_fmdir d1
new_cat_file targetfile1 catA ""
rm -fR targetfile1

fm-rm targetfile1
exit_status 0 "fm-rm"

cat_does_not_have_any_values catA
)

echo "fm-rm 3"
(
prepare_fmdir d1
new_file targetfile1

fm-rm targetfile1
exit_status 0 "fm-rm"

file_does_not_exist targetfile1
)

echo "fm-rm 4"
(
prepare_fmdir d1
new_cat_file targetfile1 catA ""

fm-rm targetfile1
exit_status 0 "fm-rm"

file_does_not_exist targetfile1

cat_does_not_have_any_values catA
)

echo "fm-rm 5"
(
prepare_fmdir d1
new_dir targetdir1

fm-rm targetdir1
exit_status 0 "fm-rm"

dir_exists targetdir1
)

echo "fm-rm 6"
(
prepare_fmdir d1
new_dir targetdir1
new_file targetdir1/targetfile1

fm-rm targetdir1
exit_status 0 "fm-rm"

file_exists targetdir1/targetfile1
)

echo "fm-rm 7"
(
prepare_fmdir d1
new_dir targetdir1
new_cat_file targetdir1/targetfile1 catA ""

fm-rm targetdir1
exit_status 0 "fm-rm"

file_exists targetdir1/targetfile1

have_the_same_cat targetdir1/targetfile1 catA
)

echo "fm-rm 8"
(
prepare_fmdir d1

fm-rm -R targetdir1
exit_status 0 "fm-rm"
)

echo "fm-rm 9"
(
prepare_fmdir d1
new_cat_file targetfile1 catA
rm -fR targetfile1

fm-rm -R targetfile1
exit_status 0 "fm-rm"

cat_does_not_have_any_values catA
)

echo "fm-rm 10"
(
prepare_fmdir d1
new_file targetfile1

fm-rm -R targetfile1
exit_status 0 "fm-rm"

file_does_not_exist targetfile1
)

echo "fm-rm 11"
(
prepare_fmdir d1
new_cat_file targetfile1 catA

fm-rm -R targetfile1
exit_status 0 "fm-rm"

file_does_not_exist targetfile1
cat_does_not_have_any_values catA
)

echo "fm-rm 12"
(
prepare_fmdir d1
new_dir targetdir1

fm-rm -R targetdir1
exit_status 0 "fm-rm"

dir_does_not_exist targetdir1
)

echo "fm-rm 14"
(
prepare_fmdir d1
new_dir targetdir1
new_file targetdir1/targetfile1

fm-rm -R targetdir1
exit_status 0 "fm-rm"

dir_does_not_exist targetdir1
)

echo "fm-rm 15"
(
prepare_fmdir d1
new_dir targetdir1
new_cat_file targetdir1/targetfile1 catA

fm-rm -R targetdir1
exit_status 0 "fm-rm"

file_does_not_exist targetdir1/targetfile1
cat_does_not_have_any_values catA
)

echo "fm-get 1"
(
prepare_fmdir d1
new_cat_file targetfile1 catA
new_cat_file targetfile2 catA
new_cat_file targetfile3 catB

fm-get catA > /dev/null 2>&1
exit_status 0 "fm-get"

fm-get catA > ./t1
cat <<-EOF | sort > ./t2
$(pwd)/targetfile1
$(pwd)/targetfile2
EOF
equal_files ./t1 ./t2
)

echo "fm-get 2"
(
prepare_fmdir d1
new_file targetfile1

fm-get catA > /dev/null 2>&1
exit_status 0 "fm-get"

if [ "x$(fm-get catA)" != "x" ]
then
	echo "path"
fi
)

echo "fm-get 3"
(
prepare_fmdir d1
new_file targetfile1
fm-set targetfile1 catA catB
new_file targetfile2
fm-set targetfile2 catA catB
new_file targetfile3
fm-set targetfile3 catA

fm-get catA catB > /dev/null 2>&1
exit_status 0 "fm-get"

fm-get catA catB | sort > ./t1
cat <<-EOF | sort > ./t2
$(pwd)/targetfile1
$(pwd)/targetfile2
EOF
equal_files ./t1 ./t2
)

echo "fm-getcat 1"
(
prepare_fmdir d1
new_cat_file targetfile1 catA ""
new_cat_file targetfile2 catB ""

fm-getcat > /dev/null 2>&1
exit_status 0 "fm-getcat"

fm-getcat | sort > ./t1
cat <<-EOF > ./t2
catA
catB
EOF
equal_files ./t1 ./t2
)

echo "fm-getcat 2"
(
prepare_fmdir d1
new_cat_file targetfile1 catA ""

fm-getcat targetfile1 > /dev/null 2>&1
exit_status 0 "fm-getcat"

fm-getcat targetfile1 > ./t1
cat <<-EOF > ./t2
catA
EOF
equal_files ./t1 ./t2
)

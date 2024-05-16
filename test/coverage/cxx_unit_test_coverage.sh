# Run c++ unit tests with ctest and summarize coverage of the files that have unit tests
src_dir=../..
build_dir=.
out_dir=${src_dir}/test/coverage/cxx-unit-tests
out_file=${out_dir}/coverage.html
mkdir -p ${out_dir}/old
mv ${out_dir}/coverage* ${out_dir}/old
params=("-j `nproc` -r" "${src_dir}" "${build_dir}" "--html" "--html-details" "${out_file}" "--sort-percentage")
in_dir=${src_dir}/test/cxx-unit-tests/tests
test_files=(`find ${in_dir} -name "ut_*.C" -type f`)
for file in "${test_files[@]}"; do
    name=$(basename "${file}" ".C")
    name=${name#ut_}
    pattern="(.+/)${name}\.[Ch]$"
    params+=("-f" "${pattern}")
done

echo "Deleting old profile info"
find . -name "*.gcda" -type f -print | xargs /bin/rm -f

ctest -j `nproc` -R cxx_unit_test

echo "Summarizing test coverage"
gcovr ${params[*]} && echo "Coverage report in file://$(realpath ${out_file})"

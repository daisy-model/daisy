# Run dai system tests and summarize coverage for all files
src_dir=../..
build_dir=.
out_dir=${src_dir}/test/coverage/dai-system-tests
out_file=${out_dir}/coverage_`date +%s`.html
mkdir -p ${out_dir}
params=("-r" "${src_dir}" "${build_dir}" "--html" "${out_file}" "--sort-percentage" "--exclude" "(.+/)ut_.+\.C$")

echo "Deleting old profile info"
find . -name "*.gcda" -type f -print | xargs /bin/rm -f

ctest -j `nproc` -R dai_system_test.

echo "Summarizing test coverage"
gcovr ${params[*]} && echo "Coverage report in file://$(realpath ${out_file})"

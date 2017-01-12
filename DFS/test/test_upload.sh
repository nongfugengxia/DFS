i=0
while [ $i -lt 2 ]; do
  ./test_upload $i &
  let i=i+1
done


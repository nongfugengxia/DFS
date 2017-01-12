i=0
while [ $i -lt 5 ]; do
  ./test_delete $i &
  let i=i+1
done


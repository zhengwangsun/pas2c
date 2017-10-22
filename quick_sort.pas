program test(input, output);
var a: array[1..101] of integer; 
k,m,tt: integer;
function partition(low,high: integer): integer;
var i, j, temp: integer;
begin
    i := low - 1;
    j := low;
    while j < high do 
    begin
        if a[j] <= a[high] then
           begin
	   	 i := i + 1;
	   	 temp := a[i];
	    	 a[i] := a[j];
	         a[j] := temp
	   end
	   else begin end; 
	j := j + 1
    end ;
    i := i + 1;
    temp := a[i];
    a[i] := a[high];
    a[high] := temp;
    partition := i
end;

procedure qs(low,high: integer);
var pivot: integer;
begin
    pivot:=0;
    if low <= high then 
       begin
             pivot := partition(low, high);
	     qs(low, pivot - 1);
	     qs(pivot + 1, high)
       end
	else begin end
end;
begin
    read(m);
    k := 1;
    while(k<=m) do 
    begin
	     read(tt);
         a[k]:=tt;
         k := k+1
    end;
    qs(1, m);
    k := 1;
    while k <= m do 
    begin
        write(a[k]);
	k := k + 1
    end
end.

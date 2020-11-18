function postfix(s,k ,n)
{
   n = length(s);
   return substr(s,n-(k-1),n);
}

{
   adr = "000" $1;
   printf "%s:",postfix(adr,4);
   for (i = 2; i < 18; i += 2) {
      dat = postfix("00" $i,2) postfix("00" $(i+1),2);
      printf " %s",dat;
   }
   printf "\n";
}

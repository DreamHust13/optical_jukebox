<center> 
<?php
	//����ip��202.114.6.69      host������:localhost
	//�û�����f103
	//���룺123456
	//���ݿ�����optical_jukebox
	//���ݱ����֣�main_caddy_address_map

   //����mysql���ݿ⣬�����ֱ�host���������������������û��������롢���ݿ��� 
   //* $db = new mysqli('localhost','f103','123456','optical_jukebox');
   mysql_connect('localhost', 'f103', '123456');
   mysql_select_db('optical_jukebox');

   //���������   
   echo "<font size=6>����ϻλ��ӳ���</font><br><br>";
 //  $query="SELECT  distinct cabinetid FROM  main_caddy_address_map";//�������ϻ��δ����
   $query="SELECT * FROM  main_caddy_address_map GROUP BY cabinetid";//��ϻ�ŵ�������
   $result=mysql_query($query);
 //$num=mysql_num_rows($result);
 //����
 //echo "<tr>num=".$num."</tr>";
 //while($libid=mysql_fetch_assoc($result))
 //echo "library id=".$libid["cabinetid"]."<br>";
//  while($libid=mysql_fetch_assoc($result))
  //  print_r($libid); 	
   while($libid=mysql_fetch_assoc($result))
{
   // print_r($libid);
   // echo "".$libid["cabinetid"]."�����߿�<br>";
    //�����һ���б���
    echo "<table border='5' width=700 height=700><tr><td>&nbsp</td>";
    for($i=0; $i<30; $i++)
           echo "<td>".$i."��</td>";
    echo "</tr>";
  //ѭ��������ݿ��main_caddy_address_map�����ݵ����
  for ($i = 0;$i<12; $i++) {
   
    echo "<tr><td>".$i."��</td>";
    for ($j = 0;$j <30; $j++)
    {
       $flag=false;
      //��sql����ȡ���ݱ�main_caddy_address_map����
       $rows = mysql_query("SELECT * FROM main_caddy_address_map");
       while($row = mysql_fetch_assoc($rows))  //row & rows 
       {
	     if( $row["row"] == $i && $row["coolumn"] == $j && $row["cabinetid"] == $libid["cabinetid"] ) 
              {
                	$flag = true;
		            break;
              } 
       }  
	   if($flag==true)
		echo "<td><font color=green>ϻ��".$row["caddyid"]."</font></td>";
	else
		echo "<td>&nbsp</td>";
    }
    echo "</tr>";
  }
  echo "<br><br>";
  echo "".$libid["cabinetid"]."�����߿�<br>";
 // echo "<br><br>";
}//while  
?>
</center>

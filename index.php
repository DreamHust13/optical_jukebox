<center> 
<?php
	//主机ip：202.114.6.69      host主机名:localhost
	//用户名：f103
	//密码：123456
	//数据库名：optical_jukebox
	//数据表名字：main_caddy_address_map

   //连接mysql数据库，参数分别：host主机名（服务器名）、用户名、密码、数据库名 
   //* $db = new mysqli('localhost','f103','123456','optical_jukebox');
   mysql_connect('localhost', 'f103', '123456');
   mysql_select_db('optical_jukebox');

   //输出表格标题   
   echo "<font size=6>光盘匣位置映射表</font><br><br>";
 //  $query="SELECT  distinct cabinetid FROM  main_caddy_address_map";//用这句盘匣号未排序
   $query="SELECT * FROM  main_caddy_address_map GROUP BY cabinetid";//盘匣号递增排序
   $result=mysql_query($query);
 //$num=mysql_num_rows($result);
 //测试
 //echo "<tr>num=".$num."</tr>";
 //while($libid=mysql_fetch_assoc($result))
 //echo "library id=".$libid["cabinetid"]."<br>";
//  while($libid=mysql_fetch_assoc($result))
  //  print_r($libid); 	
   while($libid=mysql_fetch_assoc($result))
{
   // print_r($libid);
   // echo "".$libid["cabinetid"]."号离线库<br>";
    //输出第一行列标题
    echo "<table border='5' width=700 height=700><tr><td>&nbsp</td>";
    for($i=0; $i<30; $i++)
           echo "<td>".$i."列</td>";
    echo "</tr>";
  //循环输出数据库表main_caddy_address_map的内容到表格
  for ($i = 0;$i<12; $i++) {
   
    echo "<tr><td>".$i."行</td>";
    for ($j = 0;$j <30; $j++)
    {
       $flag=false;
      //用sql语句获取数据表main_caddy_address_map数据
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
		echo "<td><font color=green>匣号".$row["caddyid"]."</font></td>";
	else
		echo "<td>&nbsp</td>";
    }
    echo "</tr>";
  }
  echo "<br><br>";
  echo "".$libid["cabinetid"]."号离线库<br>";
 // echo "<br><br>";
}//while  
?>
</center>

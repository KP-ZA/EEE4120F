// Code your design here
module fibonacci (
  //inputs
  clk, reset, start, n,	
  //outputs
  done, res);
  
  //inputs
 input clk, reset, start;
 input [5:0] n; 
  //outputs
 output reg done; 
 output reg [31:0] res; 
  
  reg [5:0] count = 0;
  reg [31:0] prev = 0; 
  reg do_start = 1'b0;
  
  always@(reset or start or posedge(clk))begin
    if (reset) begin 
      prev = 0;
      done = 0;
      do_start = 0;
      count = 0;
      res <=0;
    //  $display("reset");
    end//end reset
    else if (start) begin
      do_start = 1;
   //   $display("start");
    end//end start
    else begin
      if(do_start && count != n)begin
        if(count == 1)begin
          res <= 1;
      //    $display("count1");
        end//count = 1
        else begin
          res <= res + prev;
          prev <= res;
 //         $display("count not 1----%d  %d", res, prev);
        end//else count=1
     //  $display("%d", count);
        count = count+1;
      end//started
    end//else
  end//end always
 
endmodule

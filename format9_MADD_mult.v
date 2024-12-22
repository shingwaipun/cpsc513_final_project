module format9_MADD_mult (
    input wire [7:0] A8,
    input wire [7:0] B8,
    output wire [15:0] PAB
);

    // Declare the variables
    wire SA;
    // logic [2:0] formatA;
    wire A_is_zero;
    wire A_is_nan;
    wire A_is_inf;
    reg [6:0] EA; // 7 different bias
    reg [2:0] MA;
    wire signed [8:0] normalized_EA;
    reg [7:0] bias_a;
    // A[6:0] >= 7'b1110100 bias = 105
    // A[6:0] >= 7'b1101100 bias = 47
    // A[6:0] >= 7'b1100000 bias = 20
    // A[6:0] >= 7'b0100000 bias = 8
    // A[6:0] >= 7'b0010100 bias = 12
    // A[6:0] >= 7'b0001100 bias = 17
    // A[6:0] >= 7'b0000000 bias = 23

    always @(*) begin
        if (A8[6:2] >= 5'b11101) begin
            EA = A8[6:0];
            MA = 3'b0;
            bias_a = 8'd105;
        end else if (A8[6:2] >= 5'b11011) begin
            EA = {1'b0, A8[6:1]};
            MA = {A8[0], 2'b0};
            bias_a = 8'd47;
        end else if (A8[6:2] >= 5'b11000) begin
            EA = {2'b0, A8[6:2]};
            MA = {A8[1:0], 1'b0};
            bias_a = 8'd20;
        end else if (A8[6:2] >= 5'b01000) begin
            EA = {3'b0, A8[6:3]};
            MA = A8[2:0];
            bias_a = 8'd8;
        end else if (A8[6:2] >= 5'b00101) begin
            EA = {2'b0, A8[6:2]};
            MA = {A8[1:0], 1'b0};
            bias_a = 8'd12;
        end else if (A8[6:2] >= 5'b00011) begin
            EA = {1'b0, A8[6:1]};
            MA = {A8[0], 2'b0};
            bias_a = 8'd17;
        end else begin
            EA = A8[6:0];
            MA = 3'b0;
            bias_a = 8'd23;
        end
    end

    wire SB;
    wire B_is_zero;
    wire B_is_nan;
    wire b_is_inf;
    reg [6:0] EB; // 7 different bias
    reg [2:0] MB;
    wire signed [8:0] normalized_EB;
    reg [7:0] bias_b;

        always @(*) begin
        if (B8[6:2] >= 5'b11101) begin
            EB = B8[6:0];
            MB = 3'b0;
            bias_b = 8'd105;
        end else if (B8[6:2] >= 5'b11011) begin
            EB = {1'b0, B8[6:1]};
            MB = {B8[0], 2'b0};
            bias_b = 8'd47;
        end else if (B8[6:2] >= 5'b11000) begin
            EB = {2'b0, B8[6:2]};
            MB = {B8[1:0], 1'b0};
            bias_b = 8'd20;
        end else if (B8[6:2] >= 5'b01000) begin
            EB = {3'b0, B8[6:3]};
            MB = B8[2:0];
            bias_b = 8'd8;
        end else if (B8[6:2] >= 5'b00101) begin
            EB = {2'b0, B8[6:2]};
            MB = {B8[1:0], 1'b0};
            bias_b = 8'd12;
        end else if (B8[6:2] >= 5'b00011) begin
            EB = {1'b0, B8[6:1]};
            MB = {B8[0], 2'b0};
            bias_b = 8'd17;
        end else begin
            EB = B8[6:0];
            MB = 3'b0;
            bias_b = 8'd23;
        end
    end

    wire S_PAB;
    reg [7:0] E_PAB;
    reg [6:0] M_PAB;

    // temp variables for multiplication
    wire [3:0] MA_appended, MB_appended;
    wire signed [8:0] TEMP_E_PAB;
    wire [7:0] full_M_PAB;
    reg [15:0] temp_PAB;

    assign PAB = temp_PAB;

    // formatting for A * B
    assign SA = A8[7];

    assign A_is_zero = A8 == 8'b0;
    assign A_is_nan = A8[7:0] == 8'b10000000;
    assign A_is_inf = A8[6:0] == 7'b1111111;

    assign SB = B8[7];
    assign B_is_zero = B8 == 8'b0;
    assign B_is_nan = B8[7:0] == 8'b10000000;
    assign B_is_inf = B8[6:0] == 7'b1111111;
    
    wire [8:0] bias_c = 9'd127;

    assign MA_appended = {1'b1, MA};
    assign MB_appended = {1'b1, MB};

    assign normalized_EA = EA - bias_a;
    assign normalized_EB = EB - bias_b;

    // Multiplication
    
    assign TEMP_E_PAB = normalized_EA + normalized_EB;
    assign full_M_PAB = MA_appended * MB_appended;
    always @(*) begin
        if (full_M_PAB[7]) begin
            E_PAB = TEMP_E_PAB + bias_c + 1;
            M_PAB = full_M_PAB[6:0]; 
        end else begin
            E_PAB = TEMP_E_PAB + bias_c;
            M_PAB = {full_M_PAB[5:0], 1'b0};
        end
        
        // $display("leading_zeros: %d", leading_zeros);
        // $display("full_M_PAB: %b", full_M_PAB);
        // $display("normalized_EA: %d", normalized_EA);
        // $display("normalized_EB: %d", normalized_EB);
        // $display("temp_e_pab: %d", TEMP_E_PAB);
        // $display("E_PAB: %d", E_PAB);
        // $display("leading zeros: %d", leading_zeros);
        // $display(" ");
    end

    assign S_PAB = SA ^ SB;

    always @(*) begin
        if (A_is_nan | B_is_nan) begin
            temp_PAB = {1'b0, 15'b11111111_1000000};
        end else if (A_is_zero & B_is_inf | A_is_inf & B_is_zero) begin
            temp_PAB = {1'b0, 15'b11111111_1000000};
        end else if (A_is_inf | B_is_inf) begin
            temp_PAB = {S_PAB, 8'b11111111, 7'b0};
        end else if (A_is_zero | B_is_zero) begin
            temp_PAB = {1'b0, 8'b0, 7'b0};
        end else begin
            temp_PAB = {S_PAB, E_PAB[7:0], M_PAB};
        end
    end

    

endmodule
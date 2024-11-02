module ImportTestsArtifacts

@pub enum ExternalEnum {
    I1 = 0,
    I2,
    I3,
}

@pub class Point [x, y] {
    @pub let x, y = (x, y);

    @pub fn psum() {
        return this.x + this.y;
    }
}

@pub let X = 1;
@pub let Y = 2;

@pub fn sum(a, b) {
    return a+b;
}
